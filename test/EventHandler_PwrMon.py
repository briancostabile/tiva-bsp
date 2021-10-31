#!/bin/env python
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import struct
from EventHandler import *
import numpy as np
import os
from Sample import SampleFormat

class EventHandlerPwrMonMsgNum:
    START_REQ  = 0
    START_CNF  = 1
    STOP_REQ   = 2
    STOP_CNF   = 3
    STOP_IND   = 4
    CONFIG_REQ = 5
    CONFIG_CNF = 6
    DATA_IND   = 7
    STATS_REQ  = 8
    STATS_CNF  = 9
    CH_AVG_REQ = 10
    CH_AVG_CNF = 11
    SAMPLE_IND = 12
    CAL_REQ    = 13
    CAL_CNF    = 14

class EventHandlerPwrMonChConfigEntry:
    def __init__(self, id, shunt, name):
        self.id    = id
        self.shunt = shunt
        self.name  = name

        self.data = bytearray(struct.pack("<BL", self.id, self.shunt ))
        self.data += self.name.encode('utf8')
        self.data += ((41-len(name)) * '\0').encode('utf8')
        return

    def getBinary(self):
        return self.data


class EventHandlerPwrMonConfigReq(EventHandlerMsgReq):
    def __init__(self, chList=[], data=None):
        if data is not None:
            super(EventHandlerPwrMonConfigReq, self).__init__(data=data)
            self.numCh, = struct.unpack("<B", self.payload[0:1])

            offset = 1
            self.chList = []
            for i in range (self.numCh):
                id, shunt, name = struct.unpack("<BL41s", self.payload[offset:offset+46])
                self.chList.append(EventHandlerPwrMonChConfigEntry(id,shunt,name.decode('utf-8')))
                offset += 46
        else:
            self.numCh = len(chList)
            self.chList = chList

            data = bytearray(struct.pack("<B", self.numCh))
            for ch in self.chList:
                data += ch.getBinary()

            super(EventHandlerPwrMonConfigReq, self).__init__(EventHandlerId.PYTHON,
                                                              EventHandlerId.PWRMON,
                                                              EventHandlerPwrMonMsgNum.CONFIG_REQ,
                                                              data)
        return


class EventHandlerPwrMonConfigCnf(EventHandlerMsgCnf):
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status=0, data=None):
        if data is not None:
            super(EventHandlerPwrMonConfigCnf, self).__init__(data=data)
            self.status, = struct.unpack("<B", self.payload)
        else:
            self.status = status
            super(EventHandlerPwrMonConfigCnf, self).__init__(EventHandlerId.PYTHON,
                                                              EventHandlerId.PWRMON,
                                                              EventHandlerPwrMonMsgNum.CONFIG_CNF,
                                                              bytearray([status]))
        return


class EventHandlerPwrMonStartReq(EventHandlerMsgReq):
    def __init__(self, smplCnt=0, smplFmt=SampleFormat.FMT1_VV):
        self.smplCnt = smplCnt
        self.smplFmt = smplFmt
        data = bytearray(struct.pack("<LB", self.smplCnt, self.smplFmt))
        super(EventHandlerPwrMonStartReq, self).__init__(EventHandlerId.PYTHON,
                                                         EventHandlerId.PWRMON,
                                                         EventHandlerPwrMonMsgNum.START_REQ,
                                                         data)
        return


class EventHandlerPwrMonStartCnf(EventHandlerMsgCnf):
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status=0, data=None):
        if data is not None:
            super(EventHandlerPwrMonStartCnf, self).__init__(data=data)
            self.status = struct.unpack("<B", self.payload)
        else:
            self.status = status
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
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status=0, data=None):
        if data is not None:
            super(EventHandlerPwrMonStopCnf, self).__init__(data=data)
            self.status = struct.unpack("<B", self.payload)
        else:
            self.status = status
            super(EventHandlerPwrMonStopCnf, self).__init__(EventHandlerId.PYTHON,
                                                            EventHandlerId.PWRMON,
                                                            EventHandlerPwrMonMsgNum.STOP_CNF,
                                                            bytearray([status]))
        return


class EventHandlerPwrMonCalReq(EventHandlerMsgReq):
    def __init__(self):
        data = bytearray()
        super(EventHandlerPwrMonCalReq, self).__init__(EventHandlerId.PYTHON,
                                                       EventHandlerId.PWRMON,
                                                       EventHandlerPwrMonMsgNum.CAL_REQ,
                                                       data)
        return


class EventHandlerPwrMonCalCnf(EventHandlerMsgCnf):
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status=0, data=None):
        if data is not None:
            super(EventHandlerPwrMonCalCnf, self).__init__(data=data)
            self.status = struct.unpack("<B", self.payload)
        else:
            self.status = status
            super(EventHandlerPwrMonCalCnf, self).__init__(EventHandlerId.PYTHON,
                                                           EventHandlerId.PWRMON,
                                                           EventHandlerPwrMonMsgNum.CAL_CNF,
                                                           bytearray([status]))
        return


class EventHandlerPwrMonStopInd(EventHandlerMsgInd):
    STATUS_SUCCESS = 0
    STATUS_FAILURE = 1
    def __init__(self, status=0, data=None):
        if data is not None:
            super(EventHandlerPwrMonStopInd, self).__init__(data=data)
            self.status = struct.unpack("<B", self.payload)
        else:
            self.status = status
            super(EventHandlerPwrMonStopInd, self).__init__(EventHandlerId.PYTHON,
                                                              EventHandlerId.PWRMON,
                                                              EventHandlerPwrMonMsgNum.STOP_IND,
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
    BYTE_SIZE = 17
    def __init__(self, id=0, mvBus=0, uvShunt=0, uaShunt=0, uw=0, data=None):
        if data is not None:
            self.data = data
            self.id, self.mvBus, self.uvShunt, self.uaShunt, self.uw = struct.unpack("<Bllll", self.data )
        else:
            self.id = id
            self.mvBus  = mvBus
            self.uvShunt  = uvShunt
            self.uaShunt = uaShunt
            self.uw = uw
            self.data = bytearray(struct.pack("<Bllll", self.id, self.mvBus, self.uvShunt, self.uaShunt, self.uw))
        return

    def getBinary(self):
        return self.data

    def __str__(self):
        return f"{self.id}: mvBus:{self.mvBus} uvShunt:{self.uvShunt} uaShunt:{self.uaShunt} uw:{self.uw}"

class EventHandlerPwrMonChAvgCnf(EventHandlerMsgCnf):
    def __init__(self, channels=None, data=None):
        if data is not None:
            super(EventHandlerPwrMonChAvgCnf, self).__init__(data=data)
            self.numCh, = struct.unpack("<B", self.payload[0:1])
            self.channels = []
            offset = 1
            for i in range(self.numCh):
                self.channels.append( EventHandlerPwrMonChAvgEntry( data=self.payload[offset:(offset+EventHandlerPwrMonChAvgEntry.BYTE_SIZE)] ) )
                offset += EventHandlerPwrMonChAvgEntry.BYTE_SIZE
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
        if (len(data) == 10):
            self.data = struct.unpack("<HHHHH",data)
            self.smplFmt  = (self.data[0] >> 12) & 0xF
            self.numSmpl  = (self.data[0] >>  6) & 0x3F
            self.numCh    = (self.data[0] >>  0) & 0x3F
            self.seq      = self.data[1]
            self.chBitmap = self.data[2]
            self.startIdx = ((self.data[4] << 16) | self.data[3])
            self.smplSize = 4
        else:
            self.smplFmt  = 0
            self.numSmpl  = 0
            self.numCh    = 0
            self.seq      = 0
            self.chBitmap = 0
            self.startIdx = 0
            self.smplSize = 0
        return

    def __str__(self):
        return f"{self.seq:4d}:{self.startIdx:8d}:0x{self.chBitmap:08X}:{self.numCh}:{self.numSmpl}:{self.smplSize}"


class EventHandlerPwrMonDataInd(EventHandlerMsgInd):
    def __init__(self, data=None):
        if data is not None:
            super(EventHandlerPwrMonDataInd, self).__init__(data=data)
            self.mv_hdr = memoryview(self.payload)[0:10]
            self.hdr = PwrMonPktHdr(self.mv_hdr)
            self.dataLen = self.hdr.smplSize * (self.hdr.numCh + 1) * self.hdr.numSmpl
            self.mv_data = memoryview(self.payload)[10:10+self.dataLen]
            if(len(self.mv_data) > 0):
                try:
                    self.data = np.ndarray(shape=(self.hdr.numSmpl, (self.hdr.numCh + 1)),
                                        buffer=self.mv_data,
                                        dtype='int32',
                                        order='C')
                except:
                    print(f"Data len error len(self.mv_data):{len(self.mv_data)} expected:{4*(self.hdr.numSmpl, (self.hdr.numCh + 1))}")
            else:
                self.data = None
        return

    def __str__(self):
        s = str(self.hdr)
        s += os.linesep
        if self.data is not None:
            for smpl in range(self.hdr.numSmpl):
                for ch in range(self.hdr.numCh + 1):
                    data = self.data.item((smpl,ch))
                    s += "[{:6d}:{:6d}] ".format( int((data >> 16) & 0xFFFF), int(data & 0xFFFF) )
                s += os.linesep
        return s
