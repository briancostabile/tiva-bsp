#!/bin/env python
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import struct

class SampleFormat:
    NO_STREAM = 0
    FMT1_VV   = 1
    FMT2_P    = 2

def sampleHdr(data):
    ioBitmap, validBitmap = struct.unpack("<HH", data)
    return ioBitmap, validBitmap

def sampleFmt0(data):
    vBus, vShunt = struct.unpack("<hh", data)
    return vBus, vShunt

def sampleFmt1(data):
    power, = struct.unpack("<i", data)
    return power


# A sample represented as a numpy array of uint32
class SampleSet:
    def __init__(self, data):
        self.data = data
        self.validBitmap, self.ioBitmap = sampleHdr(self.data[0])
        return


    def hdrGet(self):
        return self.validBitmap, self.ioBitmap


    def _chNumToIdx(self, chNum):
        chBitMask = (1 << chNum)
        if (chBitMask & self.chBitmap) is 0:
            # Channel is not in the bitmap
            return -1
        else:
            chIdx = 0
            tstBitmap = 0x0001
            while chNum > 0:
                chNum -= 1
                if (tstBitmap & self.chBitmap) is not 0:
                    chIdx += 1
                tstBitmap = (tstBitmap << 1)
            return chIdx


    def chNumGet(self, chNum):
        chIdx = self._chNumToIdx(chNum)
        return self.chIdxGet(chIdx)


    def chNumIsValid(self, chNum):
       chIdx = self._chNumToIdx(chNum)
       return (((1 << chIdx) & self.validBitmap) is not 0)


    def chIdxGet(self, chIdx):
        return self.data[chIdx + 1]


    def chIdxGetFmt0(self, chIdx):
        return sampleFmt0(self.chIdxGet(chIdx))
