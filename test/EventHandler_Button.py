#!/bin/env python
# (c) Facebook, Inc. and its affiliates. Confidential and proprietary.

import struct
from EventHandler import *
import numpy as np
import os

class EventHandlerButtonMsgNum:
    PRESS_IND      = 0
    LONG_PRESS_IND = 1
    RELEASE_IND    = 2


class EventHandlerButtonInd(EventHandlerMsgInd):
    def __init__(self, msgNum=0, id=0, data=None):
        if data is not None:
            super(EventHandlerButtonInd, self).__init__(data=data)
            self.id = struct.unpack("<B",self.payload[0])
        else:
            self.id = id
            self.payload = struct.pack("<B",self.id)
            super(EventHandlerButtonInd, self).__init__(EventHandlerId.PYTHON,
                                                        EventHandlerId.BUTTON,
                                                        msgNum,
                                                        self.payload)
        return

    def __str__(self):
        s = str(self.hdr)
        s += os.linesep
        s += f"ID:{self.id}"
        return s


class EventHandlerButtonPressInd(EventHandlerButtonInd):
    def __init__(self, id=0, data=None):
        super(EventHandlerButtonPressInd, self).__init__(EventHandlerButtonMsgNum.PRESS_IND,
                                                         id, data=data)
        return


class EventHandlerButtonLongPressInd(EventHandlerButtonInd):
    def __init__(self, id=0, data=None):
        super(EventHandlerButtonLongPressInd, self).__init__(EventHandlerButtonMsgNum.LONG_PRESS_IND,
                                                             id, data=data)
        return


class EventHandlerButtonReleaseInd(EventHandlerButtonInd):
    def __init__(self, id=0, data=None):
        super(EventHandlerButtonReleaseInd, self).__init__(EventHandlerButtonMsgNum.RELEASE_IND,
                                                           id, data=data)
        return
