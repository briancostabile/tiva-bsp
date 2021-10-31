#!/bin/env python
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import struct
from multiprocessing import Manager
import threading
import logging

def decodeHexString(hex_str):
    b = bytearray()
    str_len = len(hex_str)
    for i in range(0, str_len, 2):
        b.append((int(hex_str[i], 16) << 4) | (int(hex_str[i + 1], 16) << 0))
    return b

class ProtocolCmdId:
    VERSION   = 1
    EHSTATS   = 2
    EHMSG     = 3
    BCAST_REG = 4

class ProtocolCmdHdr:
    LEN_BYTE_LEN = 2
    ID_BYTE_LEN = 2
    def __init__(self, id=None, strData=None, binData=None):
        if id != None:
            self.len = self.ID_BYTE_LEN
            self.id = id
        else:
            if (strData != None):
                binData = decodeHexString(strData)
            elif (binData == None):
                raise ValueError
            self.len, = struct.unpack("<H", binData[0:self.LEN_BYTE_LEN])
            self.id, = struct.unpack("<H", binData[self.LEN_BYTE_LEN:(self.LEN_BYTE_LEN + self.ID_BYTE_LEN)])
        return

    def getText(self):
        s = ""
        s += format(((self.len >> 0) & 0xFF), "02X")
        s += format(((self.len >> 8) & 0xFF), "02X")
        s += format(((self.id >> 0) & 0xFF), "02X")
        s += format(((self.id >> 8) & 0xFF), "02X")
        return s

    def getBinary(self):
        return bytearray( struct.pack("<HH", self.len, self.id) )


class ProtocolReq:
    STR_HDR_SOM = "$"
    STR_TRAILER = "\n\r"

    def __init__(self, id):
        self.cmdHdr = ProtocolCmdHdr(id=id)
        self.data = bytearray()
        return

    def getBinary(self):
        return self.cmdHdr.getBinary() + self.data

    def getText(self):
        s = self.STR_HDR_SOM
        s += self.cmdHdr.getText()
        for d in self.data:
            s += format(d, "02X")
        s += self.STR_TRAILER
        return s

    def __str__(self):
        return self.getText()


class ProtocolCnf:
    STR_TRAILER = "\n\r"
    STR_HDR_SOM = "!"
    STR_HDR_LEN = 9
    BIN_HDR_LEN = 4

    def __init__(self, strData=None, binData=None):
        self.valid = False
        if (strData != None) and (len(strData) >= self.STR_HDR_LEN) and (strData[0] == self.STR_HDR_SOM):
            self.valid = True
            self.cmdHdr = ProtocolCmdHdr( strData=strData[1:] )
            self.data = decodeHexString( strData[self.STR_HDR_LEN:] )

        if (binData != None):
            self.valid = True
            self.cmdHdr = ProtocolCmdHdr( binData=binData )
            self.data = binData[self.BIN_HDR_LEN:]
        return

    def getText(self):
        s = self.STR_HDR_SOM
        s += self.cmdHdr.getText()
        for d in self.data:
            s += format(d, "02X")
        s += self.STR_TRAILER
        return s

    def __str__(self):
        return self.getText()


class ProtocolCmdVersionReq(ProtocolReq):
    def __init__(self):
        super(ProtocolCmdVersionReq, self).__init__(ProtocolCmdId.VERSION)
        return


class ProtocolCmdVersionCnf(ProtocolCnf):
    def __init__(self, strData=None, binData=None):
        super(ProtocolCmdVersionCnf, self).__init__(strData=strData, binData=binData)
        if self.cmdHdr.id != ProtocolCmdId.VERSION:
            self.valid = False
        self.hwModel, self.fwVer = struct.unpack("<LL", self.data)
        return

    def __str__(self):
        s = self.__class__.__name__
        s += ": "
        s += f"hwModel:0x{self.hwModel:08X} fwVersion:0x{self.fwVer:08X}"
        return s



class ProtocolCmdEhStatsReq(ProtocolReq):
    def __init__(self):
        super(ProtocolCmdEhStatsReq, self).__init__(ProtocolCmdId.EHSTATS)
        return


class ProtocolCmdEhStatsCnf(ProtocolCnf):
    def __init__(self, strData=None, binData=None):
        super(ProtocolCmdEhStatsCnf, self).__init__(strData=strData, binData=binData)
        if self.cmdHdr.id != ProtocolCmdId.EHSTATS:
            self.valid = False
        self.numAlloc, \
        self.numFree, \
        self.numSend, \
        self.numBcast, \
        self.bcastMapTotal, \
        self.bcastMapAvail, \
        self.numEhId = struct.unpack("<LLLLBBB", self.data)
        return

    def __str__(self):
        s = self.__class__.__name__
        s += ": "
        s += f"numAlloc: {self.numAlloc}"
        s += f"numFree: {self.numFree}"
        s += f"numSend: {self.numSend}"
        s += f"numBcast: {self.numBcast}"
        s += f"bcastMapTotal: {self.bcastMapTotal}"
        s += f"bcastMapAvail: {self.bcastMapAvail}"
        s += f"numEhId: {self.numEhId}"
        return s



class ProtocolCmdBcastRegReq(ProtocolReq):
    def __init__(self, eh, msgId):
        super(ProtocolCmdBcastRegReq, self).__init__(ProtocolCmdId.BCAST_REG)
        self.eh = eh
        self.msgId = msgId
        self.data = bytearray(struct.pack("<BH", self.eh, self.msgId))
        self.cmdHdr.len += len(self.data)
        return


class ProtocolCmdBcastRegCnf(ProtocolCnf):
    def __init__(self, strData=None, binData=None):
        super(ProtocolCmdBcastRegCnf, self).__init__(strData=strData, binData=binData)
        if self.cmdHdr.id != ProtocolCmdId.BCAST_REG:
            self.valid = False
        return

    def __str__(self):
        s = self.__class__.__name__
        return s


class PrococolCmdEhMsgReq(ProtocolReq):
    def __init__(self, data):
        super(PrococolCmdEhMsgReq, self).__init__(ProtocolCmdId.EHMSG)
        self.cmdHdr.len += len(data)
        self.data += data
        return


class PrococolCmdEhMsgCnf(ProtocolCnf):
    def __init__(self, strData=None, binData=None):
        super(PrococolCmdEhMsgCnf, self).__init__(strData=strData, binData=binData)
        if self.valid and (self.cmdHdr.id != ProtocolCmdId.EHMSG):
            self.valid = False
        return

class ProtocolHandlerInfo:
    def __init__(self, manager):
        self.lock = threading.Lock()
        self.queue = manager.Queue()
        self.handler = None
        return

class ProtocolTask:
    def __init__(self, device):
        self.logger = logging.getLogger()
        self.device = device
        self.running = False
        manager = Manager()
        self.handlerTable = { ProtocolCmdId.VERSION :   ProtocolHandlerInfo(manager),
                              ProtocolCmdId.EHSTATS :   ProtocolHandlerInfo(manager),
                              ProtocolCmdId.EHMSG :     ProtocolHandlerInfo(manager),
                              ProtocolCmdId.BCAST_REG : ProtocolHandlerInfo(manager) }
        return

    # Cleanup on deletion
    def __del__(self):
        return

    def terminate(self):
        self.running = False
        return

    def versionHandler(self, handlerInfo, data):
        handlerInfo.queue.put( ProtocolCmdVersionCnf(binData=data.tobytes()) )
        return

    def version(self):
        handlerInfo = self.handlerTable[ProtocolCmdId.VERSION]
        with handlerInfo.lock:
            handlerInfo.handler = self.versionHandler
            self.device.usbBulkWrite( ProtocolCmdVersionReq().getBinary() )
            cnf = handlerInfo.queue.get()
            handlerInfo.handler = None
        return cnf

    def bcastRegisterHandler(self, handlerInfo, data):
        handlerInfo.queue.put( ProtocolCmdBcastRegCnf(binData=data.tobytes()) )
        return

    def bcastRegister(self, eh, msgId):
        handlerInfo = self.handlerTable[ProtocolCmdId.BCAST_REG]
        with handlerInfo.lock:
            handlerInfo.handler = self.bcastRegisterHandler
            self.device.usbBulkWrite( ProtocolCmdBcastRegReq(eh, msgId).getBinary() )
            cnf = handlerInfo.queue.get()
            handlerInfo.handler = None
        self.logger.info(f"Broadcast message eh:{eh} msgId:{msgId}")
        return cnf

    def registerEhMsgHandler(self, handler):
        handlerInfo = self.handlerTable[ProtocolCmdId.EHMSG]
        with handlerInfo.lock:
            handlerInfo.handler = handler
        return

    def ehMsgSend(self, msg):
        self.logger.info(f"Sending req:{msg}")
        cmd = PrococolCmdEhMsgReq(msg.data)
        self.logger.info(f"Sending cmd:{cmd}")
        self.device.usbBulkWrite( cmd.getBinary() )
        return

    # Constatly read from USB and route/handle messages as they come in
    def run(self):
        self.logger.info("ProtocolTask Running")

        self.running = True
        while self.running:
            data = self.device.usbBulkRead()
            if (data is not None):
                dataLen = len(data)
                if (dataLen >= 4):
                    offset = 0
                    mvData = memoryview(data)
                    while offset < dataLen:
                        cmdHdr = ProtocolCmdHdr(binData=mvData[offset:offset+4])
                        if cmdHdr.id in self.handlerTable:
                            handlerInfo = self.handlerTable[cmdHdr.id]
                            if handlerInfo.handler != None:
                                handlerInfo.handler(handlerInfo, mvData[offset:])
                            else:
                                self.logger.warning(f"Dropped message {mvData}")
                        else:
                            self.logger.warning(f"cmdHdr.id not in dictionary cmdHdr.id={cmdHdr.id}, dropping rest of packet {dataLen - offset}")
                            self.logger.warning(f"cmdHdr={cmdHdr}")
                            # skip remaining packet
                            offset = dataLen
                        offset += cmdHdr.len + ProtocolCmdHdr.LEN_BYTE_LEN
                else:
                    self.logger.warning(f"Short Data len:{dataLen} data:{data}")

        self.logger.info("ProtocolTask Exiting")
        return
