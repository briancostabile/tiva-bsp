#!/bin/env python
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import struct
from EventHandler import *
import numpy as np
import os

class EventHandlerPwrMonMsgNum:
    START_REQ  = 0
    START_CNF  = 1
    STOP_REQ   = 2
    STOP_CNF   = 3
    CONFIG_REQ = 4
    CONFIG_CNF = 5
    DATA_IND   = 6
    STATS_REQ  = 7
    STATS_CNF  = 8
    CH_AVG_REQ = 9
    CH_AVG_CNF = 10

class EventHandlerPwrMonChConfigEntry:
    def __init__(self, id, shunt, name):
        self.id    = id
        self.shunt = shunt
        self.name  = name

        self.data = bytearray(struct.pack("<BH", self.id, self.shunt ))
        self.data += self.name.encode('utf8')
        self.data += ((41-len(name)) * '\0').encode('utf8')
        return

    def getBinary(self):
        return self.data


class EventHandlerPwrMonConfigReq(EventHandlerMsgReq):
    def __init__(self, smplFmt, channels):
        self.smplFmt = smplFmt
        self.numChannels = len(channels)
        self.channels = channels

        data = bytearray(struct.pack("<BB",self.smplFmt, self.numChannels))
        for ch in self.channels:
            data += ch.getBinary()

        super(EventHandlerPwrMonConfigReq, self).__init__(EventHandlerId.PYTHON,
                                                          EventHandlerId.PWRMON,
                                                          EventHandlerPwrMonMsgNum.CONFIG_REQ,
                                                          data)
        return


class EventHandlerPwrMonConfigCnf(EventHandlerMsgCnf):
    def __init__(self, status=0, data=None):
        if data is not None:
            super(EventHandlerPwrMonConfigCnf, self).__init__(data=data)
        else:
            super(EventHandlerPwrMonConfigCnf, self).__init__(EventHandlerId.PYTHON,
                                                              EventHandlerId.PWRMON,
                                                              EventHandlerPwrMonMsgNum.CONFIG_CNF,
                                                              bytearray([status]))
        return


class EventHandlerPwrMonStartReq(EventHandlerMsgReq):
    def __init__(self):
        data = bytearray()
        super(EventHandlerPwrMonStartReq, self).__init__(EventHandlerId.PYTHON,
                                                         EventHandlerId.PWRMON,
                                                         EventHandlerPwrMonMsgNum.START_REQ,
                                                         data)
        return


class EventHandlerPwrMonStartCnf(EventHandlerMsgCnf):
    def __init__(self, status=0, data=None):
        if data is not None:
            super(EventHandlerPwrMonStartCnf, self).__init__(data=data)
        else:
            super(EventHandlerPwrMonStartCnf, self).__init__(EventHandlerId.PYTHON,
                                                              EventHandlerId.PWRMON,
                                                              EventHandlerPwrMonMsgNum.START_CNF,
                                                              bytearray([status]))
        return


class EventHandlerPwrMonStopReq(EventHandlerMsgReq):
    def __init__(self):
        data = bytearray()
        super(EventHandlerPwrMonStopReq, self).__init__(EventHandlerId.PYTHON,
                                                        EventHandlerId.PWRMON,
                                                        EventHandlerPwrMonMsgNum.STOP_REQ,
                                                        data)
        return


class EventHandlerPwrMonStopCnf(EventHandlerMsgCnf):
    def __init__(self, status=0, data=None):
        if data is not None:
            super(EventHandlerPwrMonStopCnf, self).__init__(data=data)
        else:
            super(EventHandlerPwrMonStopCnf, self).__init__(EventHandlerId.PYTHON,
                                                              EventHandlerId.PWRMON,
                                                              EventHandlerPwrMonMsgNum.STOP_REQ,
                                                              bytearray([status]))
        return


class EventHandlerPwrMonStatsReq(EventHandlerMsgReq):
    def __init__(self, reset=False):
        self.reset = reset
        data = bytearray(struct.pack("<B",self.reset))
        super(EventHandlerPwrMonStatsReq, self).__init__(EventHandlerId.PYTHON,
                                                         EventHandlerId.PWRMON,
                                                         EventHandlerPwrMonMsgNum.STATS_REQ,
                                                         data)
        return


class EventHandlerPwrMonStatsCnf(EventHandlerMsgCnf):
    def __init__(self, smplNum=0, smplErrNum=0, pktSndNum=0, pktErrNum=0, data=None):
        if data is not None:
            super(EventHandlerPwrMonStatsCnf, self).__init__(data=data)
            self.smplNum, self.smplErrNum, self.pktSndNum, self.pktErrNum = struct.unpack("<QLLL", self.payload)
        else:
            self.smplNum    = smplNum
            self.smplErrNum = smplErrNum
            self.pktSndNum  = pktSndNum
            self.pktErrNum  = pktErrNum
            self.payload = bytearray(struct.pack("<QLLL", self.smplNum, self.smplErrNum, self.pktSndNum, self.pktErrNum))
            super(EventHandlerPwrMonStatsCnf, self).__init__(EventHandlerId.PYTHON,
                                                             EventHandlerId.PWRMON,
                                                             EventHandlerPwrMonMsgNum.STATS_CNF,
                                                             self.payload)
        return


class EventHandlerPwrMonChAvgReq(EventHandlerMsgReq):
    def __init__(self, reset=False):
        self.reset = reset
        data = bytearray(struct.pack("<B",self.reset))
        super(EventHandlerPwrMonChAvgReq, self).__init__(EventHandlerId.PYTHON,
                                                         EventHandlerId.PWRMON,
                                                         EventHandlerPwrMonMsgNum.CH_AVG_REQ,
                                                         data)
        return


class EventHandlerPwrMonChAvgEntry:
    def __init__(self, id=0, v=0, i=0, data=None):
        if data is not None:
            self.data = data
            self.id, self.v, self.i = struct.unpack("<BHH", self.data )
        else:
            self.id = id
            self.v  = v
            self.i  = i
            self.data = bytearray(struct.pack("<BHH", self.v, self.i ))
        return

    def getBinary(self):
        return self.data


class EventHandlerPwrMonChAvgCnf(EventHandlerMsgCnf):
    def __init__(self, channels=None, data=None):
        if data is not None:
            super(EventHandlerPwrMonChAvgCnf, self).__init__(data=data)
            self.numCh, = struct.unpack("<B", self.payload[0:1])
            self.channels = []
            offset = 1
            for i in range(self.numCh):
                self.channels.append( EventHandlerPwrMonChAvgEntry( data=self.payload[offset:(offset+5)] ) )
                offset += 5
        else:
            self.payload = bytearray(struct.pack("<B",len(channels)))
            self.channels = channels
            for ch in self.channels:
                self.payload += ch.getBinary()
            super(EventHandlerPwrMonChAvgCnf, self).__init__(EventHandlerId.PYTHON,
                                                             EventHandlerId.PWRMON,
                                                             EventHandlerPwrMonMsgNum.CH_AVG_CNF,
                                                             self.payload)
        return


class PwrMonPktHdr:
    def __init__(self, data):
        if (len(data) == 12):
            self.data = struct.unpack("<LLL",data)
            self.seq      = (self.data[0] >> 16) & 0xFFFF
            self.smplFmt  = (self.data[0] >> 12) & 0xF
            self.numSmpl  = (self.data[0] >>  6) & 0x3F
            self.numCh    = (self.data[0] >>  0) & 0x3F
            self.startIdx = self.data[1]
            self.chBitmap = self.data[2]
            self.smplSize = 4
        else:
            self.seq      = 0
            self.smplSize = 0
            self.numSmpl  = 0
            self.numCh    = 0
            self.startIdx = 0
            self.chBitmap = 0
        return

    def __str__(self):
        return f"{self.seq:4d}:{self.startIdx:8d}:0x{self.chBitmap:08X}:{self.numCh}:{self.numSmpl}:{self.smplSize}"


class EventHandlerPwrMonDataInd(EventHandlerMsgInd):
    def __init__(self, data=None):
        if data is not None:
            super(EventHandlerPwrMonDataInd, self).__init__(data=data)
            self.mv_hdr = memoryview(self.payload)[0:12]
            self.hdr = PwrMonPktHdr(self.mv_hdr)
            self.dataLen = self.hdr.smplSize * self.hdr.numCh * self.hdr.numSmpl
            self.mv_data = memoryview(self.payload)[12:12+self.dataLen]
            if(len(self.mv_data) > 0):
                self.data = np.ndarray( shape=(self.hdr.numSmpl, self.hdr.numCh, 2),
                                        buffer=self.mv_data,
                                        dtype='int16',
                                        order='C' )
            else:
                self.data = None
        return

    def __str__(self):
        s = str(self.hdr)
        s += os.linesep
        if self.data is not None:
            for smpl in range(self.hdr.numSmpl):
                for ch in range(self.hdr.numCh):
                    s += "[{:6d}:{:6d}] ".format(self.data.item((smpl,ch,0)),self.data.item((smpl,ch,1)))
                s += os.linesep
        return s


