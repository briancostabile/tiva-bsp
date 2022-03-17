#!/bin/env python
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import struct
import logging

class EventHandlerId:
    BROADCAST = 0
    SERIO     = 1
    CMD       = 2
    TIMER     = 3
    LED       = 4
    PWRMON    = 5
    PYTHON    = 250

class EventHandlerMsgType:
    REQ_RSP = 0
    CNF_IND = 1

def eventHandlerGetMsgType( msgId ):
    return ((msgId >> 7) & 0x00001)

def eventHandlerGetMsgNum( msgId ):
    return ((msgId >> 0) & 0x0007F)

def eventHandlerGetMsgEhId( msgId ):
    return ((msgId >> 8) & 0x000FF)

def eventHandlerBuildMsgId( eh, type, num ):
    return ((eh << 8) | (type << 7) | num)

class EventHandlerMsgHdr:
    def __init__(self, type=0, ehSrc=0, ehDst=0, num=0, len=0, data=None):
        if data is None:
            if type == EventHandlerMsgType.REQ_RSP:
                self.eh = ehSrc
                ehId = ehDst
            else:
                self.eh = ehDst
                ehId = ehSrc
            self.msgId = eventHandlerBuildMsgId(ehId, type, num)
            self.msgType = type
            self.msgNum = num
            self.msgEh = ehId
            self.cnt = 0
            self.alloc = False
            self.data = self.setPayloadLen(len)
        else:
            self.data = data[0:6]
            self.msgId, self.eh, self.cnt, self.len = struct.unpack("<HBBH", self.data)
            self.msgType = eventHandlerGetMsgType(self.msgId)
            self.msgNum = eventHandlerGetMsgNum(self.msgId)
            self.msgEh = eventHandlerGetMsgEhId(self.msgId)
            self.alloc = ((self.cnt >> 7) & 0x01)
            self.cnt = (self.cnt & 0x7F)
        return

    def setPayloadLen(self, len):
        self.len = len + 6
        self.data = bytearray(struct.pack("<HBBH", self.msgId, self.eh, (self.cnt | (self.alloc << 7)), self.len))
        return self.data

    def getBinary(self):
        return self.data

    def __str__(self):
        s = self.__class__.__name__
        s += ": "
        s += f"msgId:0x{self.msgId:04X} eh:{self.eh} alloc:{self.alloc} cnt:{self.cnt} len:{self.len}"
        return s

class EventHandlerMsg:
    def __init__(self, type=0, ehSrc=0, ehDst=0, num=0, payload=None, data=None):
        if data is None:
            self.hdr = EventHandlerMsgHdr(type, ehSrc, ehDst, num, len(payload))
            self.payload = payload
            self.data = self.hdr.getBinary() + self.payload
        else:
            self.data = data
            self.hdr = EventHandlerMsgHdr(data=memoryview(data)[0:6])
            self.payload = memoryview(data)[6:]

        return

    def getBinary(self):
        return self.data

    def __str__(self):
        s = self.__class__.__name__
        s += ": "
        s += "\n\t" + str(self.hdr)
        s += "\n\t" + str(self.payload)
        return s


class EventHandlerMsgReq(EventHandlerMsg):
    def __init__(self, ehSrc=0, ehDst=0, num=0, payload=None, data=None):
        super(EventHandlerMsgReq, self).__init__(EventHandlerMsgType.REQ_RSP,
                                                 ehSrc, ehDst, num, payload, data)
        return

class EventHandlerMsgRsp(EventHandlerMsg):
    def __init__(self, ehSrc=0, ehDst=0, num=0, payload=None, data=None):
        super(EventHandlerMsgRsp, self).__init__(EventHandlerMsgType.REQ_RSP,
                                                 ehSrc, ehDst, num, payload, data)
        return

class EventHandlerMsgCnf(EventHandlerMsg):
    def __init__(self, ehSrc=0, ehDst=0, num=0, payload=None, data=None):
        super(EventHandlerMsgCnf, self).__init__(EventHandlerMsgType.CNF_IND,
                                                 ehSrc, ehDst, num, payload, data)
        return

class EventHandlerMsgInd(EventHandlerMsg):
    def __init__(self, ehSrc=0, ehDst=0, num=0, payload=None, data=None):
        super(EventHandlerMsgInd, self).__init__(EventHandlerMsgType.CNF_IND,
                                                 ehSrc, ehDst, num, payload, data)
        return




class EventHandler:
    def __init__(self, protocol):
        self.logger = logging.getLogger()
        self.protocol = protocol
        self.protocol.registerEhMsgHandler(self.ehMsgHandler)
        self.ehHandlerTable = {}
        self.bcastHandlerTable = {}
        self.once = True
        return

    def registerBcastMsgInd(self, eh, msgNum, handler):
        msgId = eventHandlerBuildMsgId(eh, EventHandlerMsgType.CNF_IND, msgNum)
        self.protocol.bcastRegister(EventHandlerId.PYTHON, msgId)
        self.bcastHandlerTable[msgId] = handler
        return

    def registerEhHandler(self, eh, handler):
        self.ehHandlerTable[eh] = handler
        return

    def ehMsgHandler(self, handlerInfo, data):
        msgData = data[4:]
        msgHdr = EventHandlerMsgHdr(data=msgData)
        if (msgHdr.eh == 0) and (msgHdr.msgId in self.bcastHandlerTable):
            self.bcastHandlerTable[msgHdr.msgId](msgHdr.msgId, msgData)
        elif msgHdr.eh in self.ehHandlerTable:
            self.ehHandlerTable[msgHdr.eh](msgHdr.msgId, msgData)
        else:
            self.logger.warning( f"Dropped {msgHdr}" )
        return

