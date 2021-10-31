#!/bin/env python
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

# This is the main interface class for the Lucca Power Monitor. It doesn't directly
# Communicate with the Harware; instead, it spawns off a separate process that manages
# the HW communication. That process is intended to separate the UI from the sample
# streaming and file IO necessary to get the samples off of the Hardware without dropping
# anything.
#
# An entirely separate thread (called Indexer) in the callers python-process is used to
# index the many previously run session's data files as well as monitoring the data files
# for the active session as they're written to by the Lucca Sampler Process.

import usb.core
import usb.util
import serial
import os
import sys
import time
import datetime
import threading
from pathlib import Path
import multiprocessing as mp
from EventHandler import *
from EventHandler_PwrMon import *
from Protocol import *
import logging
from Sample import SampleFormat

# By default a "samples" directory is made in the current working directory where
# the client of this fbmon python module script is being run. The module can be
# configured to put samples somewhere else. Long captures will use a lot of disk space
DEFAULT_SMPL_DIR = "samples"


# Base Class for the commands that can be sent to/from the monsoon process
class Cmd:
    READY_IND        = 0
    CONNECT_REQ      = 1
    CONNECT_CNF      = 2
    SAMPLE_START_REQ = 3
    SAMPLE_START_CNF = 4
    SAMPLE_STOP_REQ  = 5
    SAMPLE_STOP_CNF  = 6
    SHUTDOWN_REQ     = 7
    SHUTDOWN_CNF     = 8
    CONFIG_REQ       = 9
    CONFIG_CNF       = 10
    CH_AVG_REQ       = 11
    CH_AVG_CNF       = 12
    STATS_REQ        = 13
    STATS_CNF        = 14
    CAL_REQ          = 15
    CAL_CNF          = 16

    def __init__(self, data):
        self.data = data
        self.id = data[0]
        self.payload = data[1:]


# Indication Command sent by the Lucca process when the process is
# initialized and ready to receive request commands. The full path
# of the root samples directory is included in the indication.
class CmdReadyInd(Cmd):
    def __init__(self, rootSamplePath):
        super().__init__([Cmd.READY_IND, rootSamplePath])

# Request Command to connect to Lucca over USB
class CmdConnectReq(Cmd):
    def __init__(self):
        super().__init__([Cmd.CONNECT_REQ])


# Confirmation command sent my Lucca process to indicate connection
# is established
class CmdConnectCnf(Cmd):
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status):
        super().__init__([Cmd.CONNECT_CNF, status])


# Request Command to tell the Lucca process to start gathering samples
class CmdConfigReq(Cmd):
    def __init__(self, chList):
        channels = []
        for ch in chList:
            channels += [EventHandlerPwrMonChConfigEntry(ch[0], int(ch[1]*1000), ch[2])]
        data = [Cmd.CONFIG_REQ]
        data += EventHandlerPwrMonConfigReq( channels ).getBinary()
        super().__init__(data)


# Confirmation command sent my Lucca process to indicate that sampling
# has begun. The full path to the samples directory is included in
# the confirmation.
class CmdConfigCnf(Cmd):
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status):
        super().__init__([Cmd.CONFIG_CNF, status])


# Request Command to tell the Lucca process to start gathering samples
class CmdSampleStartReq(Cmd):
    def __init__(self, smplCnt=0, smplFmt=SampleFormat.FMT1_VV):
        super().__init__([Cmd.SAMPLE_START_REQ, smplCnt, smplFmt])


# Confirmation command sent my Lucca process to indicate that sampling
# has begun. The full path to the samples directory is included in
# the confirmation.
class CmdSampleStartCnf(Cmd):
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status, samplePath):
        super().__init__([Cmd.SAMPLE_START_CNF, status, samplePath])


# Request Command to tell the Lucca process to stop gathering samples
class CmdSampleStopReq(Cmd):
    def __init__(self):
        super().__init__([Cmd.SAMPLE_STOP_REQ])


# Confirmation Command sent by the Lucca process to indicate that
# sampling has stopped
class CmdSampleStopCnf(Cmd):
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status):
        super().__init__([Cmd.SAMPLE_STOP_CNF, status])


# Request Command to tell the Lucca process to stop gathering samples
class CmdCalReq(Cmd):
    def __init__(self):
        super().__init__([Cmd.CAL_REQ])


# Confirmation Command sent by the Lucca process to indicate that
# sampling has stopped
class CmdCalCnf(Cmd):
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status):
        super().__init__([Cmd.CAL_CNF, status])


# Request Command to tell the Lucca process to stop gathering samples
class CmdChAvgReq(Cmd):
    def __init__(self, reset):
        super().__init__([Cmd.CH_AVG_REQ, reset])


# Confirmation Command sent by the Lucca process with the channel averages
class CmdChAvgCnf(Cmd):
    def __init__(self, numCh, channels):
        super().__init__([Cmd.CH_AVG_CNF, numCh, channels])


# Request Command to shut down operations and cleanup any resources
# A flag in the request can be used to keep the power supply on
# after the shutdown (no sampling will occur)
class CmdShutdownReq(Cmd):
    def __init__(self, keepPwrOn):
        super().__init__([Cmd.SHUTDOWN_REQ, keepPwrOn])


# Confirmation command from the Lucca process that the sampling
# has stopped and the data files are all closed.
class CmdShutdownCnf(Cmd):
    def __init__(self):
        super().__init__([Cmd.SHUTDOWN_CNF])



# Collect all Lucca interfaces into this class. This instantiates either a real or virtual
# Lucca device. The real device uses the USB Interface to communicate with the hardware while
# the virtual one spoofs the Lucca library API but pulls samples from a previously saved off
# sample session
class Device:
    VENDOR_SPECIFIC_IF_ID = 0xFF
    VENDOR_ID_DEFAULT  = 0x1CBE
    PRODUCT_ID_DEFAULT = 0x0007

    BULK_EP_IN_MAX = 4096
    BULK_EP_IN_TIMEOUT_MS = 100
    BULK_EP_OUT_TIMEOUT_MS = 100

    CDC_SERIAL_PORT_FORMAT_STRING_OSX = "/dev/cu.usbmodem{}1"
    CDC_SERIAL_PORT_BAUD_RATE = 921600
    CDC_SERIAL_PORT_BYTE_SIZE = serial.EIGHTBITS
    CDC_SERIAL_PORT_PARITY    = serial.PARITY_NONE
    CDC_SERIAL_PORT_STOP_BITS = serial.STOPBITS_ONE

    SAMPLING_FREQ_HZ = 2500

    PwrMonDataIndMsgId = eventHandlerBuildMsgId(EventHandlerId.PWRMON,
                                                EventHandlerMsgType.CNF_IND,
                                                EventHandlerPwrMonMsgNum.DATA_IND)


    def __init__(self, samplesRootPath=None, samplesPath=None):
        self.connected = False
        self.pwrMonEh = None
        self.eventHandler = None
        self.reqLock = threading.Lock()
        manager = Manager()
        self.cnfQueue = manager.Queue()
        self.dataIndCnt = 0
        self.dataIndErrCnt = 0
        self.samplesRootPath = samplesRootPath
        self.samplesPath = samplesPath
        self.sampling = False
        self.smplHandler = None
        self.logger = logging.getLogger()
        self.logger.setLevel(logging.INFO)

        if samplesPath is not None:
            #self.lucca = VirtualLucca(os.path.join(samplesRootPath, samplesPath))
            self.connected = True
            self.logger.info(f"Using virtual Lucca with samples located at {samplesPath}")

        return

    def secondsToSampleSets(self, seconds):
        return seconds * self.SAMPLING_FREQ_HZ

    def usbBulkConnect(self, usbDev):
        # Find the Vendor-Specific Bulk Endpoint interface
        activeConfig = usbDev.get_active_configuration()
        for i in activeConfig:
            if i.bInterfaceClass == self.VENDOR_SPECIFIC_IF_ID:
                bulkIntf = i
                break

        if bulkIntf is None:
            raise ValueError('USB Vendor Specific Interface {} Not found')

        # Get the Bulk Out endpoint
        bulkEpOut = usb.util.find_descriptor( bulkIntf,
            # match the first OUT endpoint
            custom_match = \
                lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)

        assert bulkEpOut is not None

        # Get the Bulk In endpoint
        bulkEpIn = usb.util.find_descriptor( bulkIntf,
            # match the first OUT endpoint
            custom_match = \
                lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)

        assert bulkEpIn is not None

        data = []
        flushCnt = 0
        while data is not None:
            data = self.usbBulkRead()
            if data is not None:
                flushCnt += len(data)

        self.logger.info(f"Bulk USB Connected {flushCnt} bytes flushed")
        return bulkEpIn, bulkEpOut


    def usbBulkRead(self):
        data = None
        if hasattr(self, 'bulkEpIn') and self.bulkEpIn is not None:
            try:
                data = self.bulkEpIn.read(self.BULK_EP_IN_MAX, self.BULK_EP_IN_TIMEOUT_MS)
            except usb.core.USBTimeoutError as err:
                pass
            except usb.core.USBError:
                self.connected = False
                self.usbDev = None
                self.bulkEpIn = None
                self.bulkEpOut = None
                self.logger.warning(f"Bulk USB Disconnected")
                pass
        return data


    def usbBulkWrite(self, data):
        if hasattr(self, 'bulkEpOut') and self.bulkEpOut is not None:
            try:
                self.bulkEpOut.write(data, self.BULK_EP_OUT_TIMEOUT_MS)
            except usb.core.USBTimeoutError as err:
                pass
        return


    def usbCdcConnect(self, port):
        try:
            cdcPort = serial.Serial( port=port,
                                     baudrate=self.CDC_SERIAL_PORT_BAUD_RATE,
                                     bytesize=self.CDC_SERIAL_PORT_BYTE_SIZE,
                                     parity=self.CDC_SERIAL_PORT_PARITY,
                                     stopbits=self.CDC_SERIAL_PORT_STOP_BITS,
                                     timeout=0.1,
                                     xonxoff=False,
                                     rtscts=False,
                                     write_timeout=None,
                                     dsrdtr=False,
                                     inter_byte_timeout=None,
                                     exclusive=None )
            self.logger.info(f"USB CDC port Connected {self.CDC_SERIAL_PORT_BAUD_RATE} {self.CDC_SERIAL_PORT_BYTE_SIZE}:{self.CDC_SERIAL_PORT_PARITY}:{self.CDC_SERIAL_PORT_STOP_BITS}" )

        except serial.serialutil.SerialException as err:
            cdcPort = None
            self.logger.warning( "USB CDC port Connection Failed" )

        return cdcPort


    def usbConnect(self, eventHandler, vendorId=VENDOR_ID_DEFAULT, productId=PRODUCT_ID_DEFAULT):
        self.connected = False
        self.eventHandler = eventHandler
        self.vendorId  = vendorId
        self.productId = productId

        # Find the device in the USB connection tree
        self.usbDev = usb.core.find(idVendor=self.vendorId, idProduct=self.productId)

        if self.usbDev is None:
            self.logger.info(f'USB Device not found vendor:0x{self.vendorId:04X} product:0x{self.productId:04X}')
            return self.connected

        # set the active configuration. With no arguments, the first
        # configuration will be the active one
        self.usbDev.set_configuration()

        try:
            self.logger.info(f"Manufacturer: {self.usbDev.manufacturer}" )
            self.logger.info(f"Product: {self.usbDev.product}" )
            self.logger.info(f"Serial Number: {self.usbDev.serial_number}")
        except:
            self.logger.error("Unable to communicate over USB")
            return False

        #self.cdcPort = self.usbCdcConnect(self.CDC_SERIAL_PORT_FORMAT_STRING_OSX.format(self.usbDev.serial_number))
        self.bulkEpIn, self.bulkEpOut = self.usbBulkConnect(self.usbDev)
        self.connected = (self.bulkEpIn is not None) and (self.bulkEpOut is not None)

        if self.connected:
            # register for Event Handler Messages
            self.eventHandler.registerBcastMsgInd(EventHandlerId.PWRMON,
                                                  EventHandlerPwrMonMsgNum.DATA_IND,
                                                  self.pwrMonDataIndHandler)
            self.eventHandler.registerBcastMsgInd(EventHandlerId.PWRMON,
                                                  EventHandlerPwrMonMsgNum.STOP_IND,
                                                  self.pwrMonStopIndHandler)
            self.eventHandler.registerEhHandler(EventHandlerId.PYTHON, self.msgHandler)

            # Make sure sampling is stopped
            self.sampleStop()

            # Grab the Device version information
            cnf = self.eventHandler.protocol.version()
            self.hwModel = cnf.hwModel
            self.fwVer = cnf.fwVer
            self.logger.info(f"hwModel:0x{cnf.hwModel:08X} fwVer:0x{cnf.fwVer:08X}")

        return self.connected

    # Cleanup threads on delete
    def __del__(self):
        self.shutdown()
        return

    def sendReqWaitForCnf(self, req):
        with self.reqLock:
            self.eventHandler.protocol.ehMsgSend(req)
            cnf = self.cnfQueue.get()
        return cnf

    def calibrate(self):
        if not self.connected:
            return False
        cnfData = self.sendReqWaitForCnf(EventHandlerPwrMonCalReq())
        msgCnf = EventHandlerPwrMonCalCnf(data=cnfData)
        return (msgCnf.status == EventHandlerPwrMonCalCnf.STATUS_SUCCESS)

    def config(self, req):
        if not self.connected:
            return False
        cnfData = self.sendReqWaitForCnf(req)
        msgCnf = EventHandlerPwrMonConfigCnf(data=cnfData)
        return (msgCnf.status == EventHandlerPwrMonConfigCnf.STATUS_SUCCESS)

    def sampleStart(self, smplCnt=0, smplFmt=SampleFormat.FMT1_VV):
        if not self.connected:
            return False

        cnfData = self.sendReqWaitForCnf(EventHandlerPwrMonStartReq(smplCnt, smplFmt))
        msgCnf = EventHandlerPwrMonStartCnf(data=cnfData)
        if (msgCnf.status == EventHandlerPwrMonStartCnf.STATUS_SUCCESS):
            self.sampling = True
        return (self.sampling == True)

    def sampleStop(self):
        if not self.connected:
            return False
        cnfData = self.sendReqWaitForCnf(EventHandlerPwrMonStopReq())
        msgCnf = EventHandlerPwrMonStopCnf(data=cnfData)
        if (msgCnf.status == EventHandlerPwrMonStopCnf.STATUS_SUCCESS):
            self.sampling = False
        return (self.sampling == False)

    def chAvg(self, reset=False):
        if not self.connected:
            return False
        cnfData = self.sendReqWaitForCnf(EventHandlerPwrMonChAvgReq(reset))
        msgCnf = EventHandlerPwrMonChAvgCnf(data=cnfData)
        chAvgList = []
        for ch in msgCnf.channels:
            chAvgList.append(EventHandlerPwrMonChAvgEntry(data=ch.data.tobytes()))
        return msgCnf.numCh, chAvgList

    def shutdown(self, vEnable=True):
        if self.sampling == True:
            self.sampleStop()
            self.sampling = False
        return

    def msgHandler(self, msgId, msg):
        self.cnfQueue.put(msg.tobytes())
        return

    def pwrMonDataIndHandler(self, msgId, msg):
        try:
            dataInd = EventHandlerPwrMonDataInd(data=msg)
            if self.smplHandler is not None:
                self.smplHandler.processDataInd(dataInd)
            self.dataIndCnt += 1
        except:
            self.dataIndErrCnt += 1
            print(f"Corrupted Packet cnt:{self.dataIndErrCnt}")
            pass
        # if self.dataIndCnt % 100 == 0:
        #     print(f"DataInd:{self.dataIndCnt}\n\t{dataInd}")
        return

    def pwrMonStopIndHandler(self, msgId, msg):
        stopInd = EventHandlerPwrMonStopInd(data=msg)
        self.logger.info(f"StopInd:{stopInd}")
        if self.smplHandler is not None:
            self.smplHandler.flushBuffer()
        return


# This is the main task and command loop for the Lucca device
def DeviceProcess(conn, sampleRootPath, samplesPath):
    lucca = Device(sampleRootPath, samplesPath)

    # The protocol task is looping forever on the USB bulk input
    protocolTask = ProtocolTask(lucca)
    protocolThread = threading.Thread(target=protocolTask.run)
    eventHandler = EventHandler(protocolTask)
    protocolThread.start()

    # Breifly wait for the task to start running before returning
    while protocolTask.running is False:
        time.sleep(0.01)

    conn.send(CmdReadyInd(lucca.samplesRootPath).data)

    # Loop forever handling commands until SHUTDOWN
    # Look for keyboard interrupt
    running = True
    try:
        while running:
            cmd = Cmd(conn.recv())
            if cmd.id == Cmd.CONNECT_REQ:
                if lucca.usbConnect(eventHandler):
                    status = CmdConnectCnf.STATUS_SUCCESS
                else:
                    status = CmdConnectCnf.STATUS_FAILURE
                conn.send(CmdConnectCnf(status).data)

            elif cmd.id == Cmd.CONFIG_REQ:
                if lucca.config(EventHandlerPwrMonConfigReq(data=bytearray(cmd.payload))):
                    status = CmdConfigCnf.STATUS_SUCCESS
                else:
                    status = CmdConfigCnf.STATUS_FAILURE
                conn.send(CmdConfigCnf(status).data)

            elif cmd.id == Cmd.SAMPLE_START_REQ:
                if lucca.sampleStart(cmd.payload[0], cmd.payload[1]):
                    status = CmdSampleStartCnf.STATUS_SUCCESS
                else:
                    status = CmdSampleStartCnf.STATUS_FAILURE
                if lucca.smplHandler is not None:
                    p = lucca.smplHandler.sampleDirPath
                else:
                    p = ""
                conn.send(CmdSampleStartCnf(status, p).data)

            elif cmd.id == Cmd.SAMPLE_STOP_REQ:
                if lucca.sampleStop():
                    status = CmdSampleStopCnf.STATUS_SUCCESS
                else:
                    status = CmdSampleStopCnf.STATUS_FAILURE
                conn.send(CmdSampleStopCnf(status).data)

            elif cmd.id == Cmd.CH_AVG_REQ:
                numCh, channels = lucca.chAvg(cmd.payload[0])
                #print(f"Channel Averages:   numCh:{numCh} channels:{channels}")
                conn.send(CmdChAvgCnf(numCh, channels).data)

            elif cmd.id == Cmd.CAL_REQ:
                if lucca.calibrate():
                    status = CmdSampleStartCnf.STATUS_SUCCESS
                else:
                    status = CmdSampleStartCnf.STATUS_FAILURE
                conn.send(CmdCalCnf(status).data)

            elif cmd.id == Cmd.SHUTDOWN_REQ:
                conn.send(CmdShutdownCnf().data)
                break

    # Clean up the resources on a keyboard interrupt
    except KeyboardInterrupt:
        print(
            "\n[{}]: Keyboard interrupt detected while receiving connection".format(
                datetime.datetime.now()
            )
        )

    protocolTask.terminate()
    sys.exit(0)
    return



# Wrapper class for the Lucca device. This class handles the sampler thread startup
# and shutdown as well as wrapping the underlying Lucca device class.
class DeviceApi:
    def __init__(self, samplesRootPath=None, samplesPath=None):
        self.logger = logging.getLogger()
        # Create default dir for the samples
        if samplesRootPath is None:
            samplesRootPath = os.path.join(Path.home(), DEFAULT_SMPL_DIR)

        # Spin up the Device process
        mp.set_start_method("spawn")
        self.parentConn, self.childConn = mp.Pipe()
        self.process = mp.Process(
            target=DeviceProcess, args=(self.childConn, samplesRootPath, samplesPath)
        )
        self.process.start()

        # Wait for the process to launch, the lucca to be configured and ready
        # to receive commands
        self.logger.info("Waiting for Lucca process to start")
        cmd = Cmd(self.parentConn.recv())
        if cmd.id == Cmd.READY_IND:
            samplesRootPath = cmd.payload[0]
            self.logger.info("Lucca Process Ready")
        else:
            self.logger.warning(f"Unexpected Response id:{cmd.id}")
            # Todo: generate Exception

        self.samplesRootPath = samplesRootPath
        self.samplesPath = samplesPath
        self.connect()
        return

    # Cleanup threads on delete
    def __del__(self):
        self.process.terminate()
        return

    def sendCmdWaitForCmd(self, req):
        self.parentConn.send(req.data)
        cmd = Cmd(self.parentConn.recv())
        if cmd.id != (req.id + 1):
            self.logger.warning(f"Unexpected Response id:{cmd.id}")
            cmd = None
        return cmd

    def connect(self):
        cmd = self.sendCmdWaitForCmd(CmdConnectReq())
        if (cmd is not None) and (cmd.payload[0] == CmdConnectCnf.STATUS_SUCCESS):
            self.logger.info("Lucca Connected")
        return

    def config(self,
               chList=[(9, .1, "MAIN_PWR")]):
        cmd = self.sendCmdWaitForCmd(CmdConfigReq(chList))
        status = cmd.payload[0]
        if status == CmdConfigCnf.STATUS_SUCCESS:
            self.logger.info("Lucca Configured")
        else:
            self.logger.warning("Lucca Configuration Failed")
        return

    def calibrate(self):
        cmd = self.sendCmdWaitForCmd(CmdCalReq())
        if (cmd is not None) and (cmd.payload[0] == CmdCalCnf.STATUS_SUCCESS):
            self.logger.info("Lucca Calibrated")
        return

    def sampleStart(self, seconds=0, smplFmt=SampleFormat.FMT1_VV):
        smplCnt = (Device.SAMPLING_FREQ_HZ * seconds)
        cmd = self.sendCmdWaitForCmd(CmdSampleStartReq(smplCnt, smplFmt))
        if (cmd is not None):
            self.logger.info(f"Lucca Sample Start {cmd.payload[0]} {cmd.payload[1]}")
        return cmd.payload[1]

    def sampleStop(self):
        cmd = self.sendCmdWaitForCmd(CmdSampleStopReq())
        if (cmd is not None):
            self.logger.info("Lucca Sample Stop")
        return

    def chAvg(self, reset=False):
        cmd = self.sendCmdWaitForCmd(CmdChAvgReq(reset))
        if (cmd is not None):
            self.logger.info(f"Lucca Channel Average")
        return cmd.payload[0], cmd.payload[1]

    def shutdown(self, keepPwrOn):
        cmd = self.sendCmdWaitForCmd(CmdShutdownReq(keepPwrOn))
        if cmd is not None:
            self.logger.info("Lucca Shutdown")
        return




# For testing, this module can be run standalone.
def main():
    import cProfile
    import io
    import pstats
    from pathlib import Path

    pr = cProfile.Profile()

    # Create default dir for the samples
    samplesRootPath = None #os.path.join(Path.home(), "samples")
    samplesPath = None  # "21-02-05/07-21-15"

    device = DeviceApi(samplesRootPath, samplesPath)  # Can pass in a sample root-path
    device.config( [(0, .01, "0V74_PMIC_VREG_S1A_SOC"),
                    (1, .01, "0V75_PMIC_VREG_S2A_SOC"),
                    (2, .01, "0V74_PMIC_VREG_S3A_SOC"),
                    (3, .01, "2V04_PMIC_VREG_S4A_PMIC"),
                    (4, .01, "1V35_PMIC_VREG_S5A_PMIC"),
                    (5, .01, "PMIC_VPH_PWR_WIFI"),
                    (6, .01, "PMIC_VPH_PWR_DBG"),
                    (7, .01, "VPH_PWR_LTE"),
                    (9, .01, "MAIN_PWR")] )
    # device.config( [(0, .01, "0V74_PMIC_VREG_S1A_SOC")] )
    time.sleep(2)
    print("Begin Sampling")
    pr.enable()
    device.sampleStart(10, SampleFormat.NO_STREAM)
    pr.disable()
    time.sleep(12)
    print("End Sampling")
    device.sampleStop()
    device.shutdown(True)

    s = io.StringIO()
    sortby = "cumulative"
    ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
    ps.print_stats()
    print(s.getvalue())

    sys.exit(0)


if __name__ == "__main__":
    main()
