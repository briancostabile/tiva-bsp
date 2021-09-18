import usb.core
import usb.util
import struct
import os
import serial
from Protocol import (
    ProtocolCmdVersionReq,
    ProtocolCmdVersionCnf,
    PrococolCmdEhMsgReq,
    PrococolCmdEhMsgCnf,
    ProtocolCmdBcastRegReq,
    ProtocolCmdBcastRegCnf
)

from EventHandler_Button import (
    EventHandlerButtonMsgNum,
)

from EventHandler_PwrMon import (
    EventHandlerPwrMonMsgNum,
    EventHandlerPwrMonStartReq,
    EventHandlerPwrMonStartCnf,
    EventHandlerPwrMonStopReq,
    EventHandlerPwrMonStopCnf,
    EventHandlerPwrMonConfigReq,
    EventHandlerPwrMonConfigCnf,
    EventHandlerPwrMonChConfigEntry,
    EventHandlerPwrMonStatsReq,
    EventHandlerPwrMonStatsCnf,
    EventHandlerPwrMonChAvgReq,
    EventHandlerPwrMonChAvgEntry,
    EventHandlerPwrMonChAvgCnf,
    EventHandlerPwrMonDataInd
)

from EventHandler import (
    EventHandlerMsg,
    EventHandlerMsgId,
    EventHandlerMsgType,
    EventHandlerId
)



class TivaUsb:
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


    def __init__(self, vendorId=VENDOR_ID_DEFAULT, productId=PRODUCT_ID_DEFAULT):
        self.vendorId  = vendorId
        self.productId = productId

        # Find the device in the USB connection tree
        self.usbDev = usb.core.find(idVendor=self.vendorId, idProduct=self.productId)

        if self.usbDev is None:
            raise ValueError(f'USB Device not found vendor:0x{self.vendorId:04X} product:0x{self.productId:04X}')

        # set the active configuration. With no arguments, the first
        # configuration will be the active one
        self.usbDev.set_configuration()

        print(f"Manufacturer: {self.usbDev.manufacturer}" )
        print(f"Product: {self.usbDev.product}" )
        print(f"Serial Number: {self.usbDev.serial_number}")

        self.cdcIntf = None
        self.cdcPortName = self.CDC_SERIAL_PORT_FORMAT_STRING_OSX.format(self.usbDev.serial_number)

        self.bulkIntf  = None
        self.bulkEpOut = None
        self.bulkEpIn  = None

        return


    def bulkConnect(self):
        # Find the Vendor-Specific Bulk Endpoint interface
        for i in self.usbDev.get_active_configuration():
            if i.bInterfaceClass == self.VENDOR_SPECIFIC_IF_ID:
                self.bulkIntf = i
                break

        if self.bulkIntf is None:
            raise ValueError('USB Vendor Specific Interface {} Not found')

        # Get the Bulk Out endpoint
        self.bulkEpOut = usb.util.find_descriptor( self.bulkIntf,
            # match the first OUT endpoint
            custom_match = \
                lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT )

        assert self.bulkEpOut is not None

        # Get the Bulk In endpoint
        self.bulkEpIn = usb.util.find_descriptor( self.bulkIntf,
            # match the first OUT endpoint
            custom_match = \
                lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)

        assert self.bulkEpIn is not None
        print(f"Bulk USB Connected" )
        return


    def bulkRead(self):
        data = bytearray()
        try:
            data = self.bulkEpIn.read(self.BULK_EP_IN_MAX, self.BULK_EP_IN_TIMEOUT_MS)
        except usb.core.USBTimeoutError as err:
            pass
        return data


    def bulkWrite(self, data):
        try:
            self.bulkEpOut.write(data, self.BULK_EP_OUT_TIMEOUT_MS)
        except usb.core.USBTimeoutError as err:
            pass
        return


    def cdcConnect(self):
        try:
            self.cdcPort = serial.Serial( port=self.cdcPortName,
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
            print(f"USB CDC port Connected {self.CDC_SERIAL_PORT_BAUD_RATE} {self.CDC_SERIAL_PORT_BYTE_SIZE}:{self.CDC_SERIAL_PORT_PARITY}:{self.CDC_SERIAL_PORT_STOP_BITS}" )
        except serial.serialutil.SerialException as err:
            print( "USB CDC port Connection Failed" )

        return


    def cdcDisconnect(self):
        if self.cdcPort is not None:
            self.cdcPort.close()
        print(f"USB CDC port Closed" )
        return

    # Wrapper to encode the command and wait for resulting bytes
    def cdcSendWaitForRsp(self, cmd):
        print(f"Sending Cmd-str:{cmd}")
        self.cdcPort.write(str(cmd).encode("utf-8"))
        rspBytes = self.cdcPort.read_until()
        rspStr = rspBytes.decode("utf-8").strip("\n").strip("\r")
        print(f"Received:{rspStr}")
        return rspStr


# #send in command to trigger a response
# msg = bytearray([0x03,0x12,0x13,0xFF])
# ep_out.write(msg, 100)


def config(tivaUsb):
    channels = [ EventHandlerPwrMonChConfigEntry(0,10,"TEST_CHANNEL_0"),
                 EventHandlerPwrMonChConfigEntry(1,100,"TEST_CHANNEL_1") ]
    req = EventHandlerPwrMonConfigReq(0,channels)
    print(req)
    cmd = PrococolCmdEhMsgReq( req.getBinary() )
    print( cmd )

    tivaUsb.bulkWrite( cmd.getBinary() )
    rsp = tivaUsb.bulkRead()
    print(rsp)
    cnf = PrococolCmdEhMsgCnf(binData=rsp)
    print(cnf)
    msg = EventHandlerMsg(data=cnf.data)
    print(msg)
    msgCnf = EventHandlerPwrMonConfigCnf(data=cnf.data)
    print(msgCnf)
    return

def start(tivaUsb):
    req = EventHandlerPwrMonStartReq()
    print(req)
    cmd = PrococolCmdEhMsgReq( req.getBinary() )
    print( cmd )

    tivaUsb.bulkWrite( cmd.getBinary() )
    rsp = tivaUsb.bulkRead()
    print(rsp)
    cnf = PrococolCmdEhMsgCnf(binData=rsp)
    print(cnf)
    msg = EventHandlerMsg(data=cnf.data)
    print(msg)
    msgCnf = EventHandlerPwrMonStartCnf(data=cnf.data)
    print(msgCnf)
    return

def stop(tivaUsb):
    req = EventHandlerPwrMonStopReq()
    print(req)
    cmd = PrococolCmdEhMsgReq( req.getBinary() )
    print( cmd )

    tivaUsb.bulkWrite( cmd.getBinary() )
    rsp = tivaUsb.bulkRead()
    print(rsp)
    cnf = PrococolCmdEhMsgCnf(binData=rsp)
    print(cnf)
    msg = EventHandlerMsg(data=cnf.data)
    print(msg)
    msgCnf = EventHandlerPwrMonStopCnf(data=cnf.data)
    print(msgCnf)
    return

def getStats(tivaUsb, reset):
    req = EventHandlerPwrMonStatsReq(reset=reset)
    print(req)
    cmd = PrococolCmdEhMsgReq( req.getBinary() )
    print( cmd )

    tivaUsb.bulkWrite( cmd.getBinary() )
    rsp = tivaUsb.bulkRead()
    print(rsp)
    cnf = PrococolCmdEhMsgCnf(binData=rsp)
    print(cnf)
    msg = EventHandlerMsg(data=cnf.data)
    print(msg)
    msgCnf = EventHandlerPwrMonStatsCnf(data=cnf.data)
    print(msgCnf)
    return


def getChAvg(tivaUsb, reset):
    req = EventHandlerPwrMonChAvgReq(reset=reset)
    print(req)
    cmd = PrococolCmdEhMsgReq( req.getBinary() )
    print( cmd )

    tivaUsb.bulkWrite( cmd.getBinary() )
    rsp = tivaUsb.bulkRead()
    print(rsp)
    cnf = PrococolCmdEhMsgCnf(binData=rsp)
    print(cnf)
    msg = EventHandlerMsg(data=cnf.data)
    print(msg)
    msgCnf = EventHandlerPwrMonChAvgCnf(data=cnf.data)
    print(msgCnf)
    return


def registerForInd(tivaUsb, msgId):
    req = ProtocolCmdBcastRegReq( EventHandlerId.PYTHON, msgId )
    print(req)
    tivaUsb.bulkWrite( req.getBinary() )
    rsp = tivaUsb.bulkRead()
    cnf = ProtocolCmdBcastRegCnf(binData=rsp)
    print(cnf)
    return

def registerForPwrMonDataInd(tivaUsb):
    registerForInd(tivaUsb,
                   EventHandlerMsgId(EventHandlerId.PWRMON,
                                     EventHandlerMsgType.CNF_IND,
                                     EventHandlerPwrMonMsgNum.DATA_IND).msgId)
    return

def registerForButtonInd(tivaUsb):
    registerForInd(tivaUsb,
                   EventHandlerMsgId(EventHandlerId.PWRMON,
                                     EventHandlerMsgType.CNF_IND,
                                     EventHandlerButtonMsgNum.PRESS_IND).msgId)
    registerForInd(tivaUsb,
                   EventHandlerMsgId(EventHandlerId.PWRMON,
                                     EventHandlerMsgType.CNF_IND,
                                     EventHandlerButtonMsgNum.LONG_PRESS_IND).msgId)
    registerForInd(tivaUsb,
                   EventHandlerMsgId(EventHandlerId.PWRMON,
                                     EventHandlerMsgType.CNF_IND,
                                     EventHandlerButtonMsgNum.RELEASE_IND).msgId)
    return


def main():
    tivaUsb = TivaUsb()
    tivaUsb.bulkConnect()
    # tivaUsb.cdcConnect()
    # # str_data = tivaUsb.cdcSendWaitForRsp(ProtocolCmdVersionReq())
    # # rsp = ProtocolCmdVersionCnf( str_data=str_data )
    # # print(rsp)

    registerForPwrMonDataInd(tivaUsb)
    registerForButtonInd(tivaUsb)
    #config(tivaUsb)
    #getStats(tivaUsb,False)
    #getChAvg(tivaUsb,False)
    #start(tivaUsb)

    print("waiting for Data Indiation")
    for i in range(5):
        rsp = bytearray()
        while len(rsp) == 0:
            rsp = tivaUsb.bulkRead()

        print(rsp)
        # cnf = PrococolCmdEhMsgCnf(binData=rsp)
        # print(cnf)
        # msg = EventHandlerMsg(data=cnf.data)
        # print(msg)
        # msgInd = EventHandlerPwrMonDataInd(data=cnf.data)
        # print(msgInd)

    #stop(tivaUsb)
    #getStats(tivaUsb,False)
    #getChAvg(tivaUsb,False)

if __name__ == "__main__":
    main()
