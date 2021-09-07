import usb.core
import usb.util
import struct
import numpy as np
import os

dev = usb.core.find(idVendor=0x1cbe, idProduct=0x0007)

# was it found?
if dev is None:
    raise ValueError('Device not found')

# set the active configuration. With no arguments, the first
# configuration will be the active one
dev.set_configuration()

print(dev)
print("--------------------------------------")

# Find the Vendor-Specific Bulk Endpoint interface
intf = None
for i in dev.get_active_configuration():
    if i.bInterfaceClass == 0xFF:
        intf = i
        break

if intf is None:
    raise ValueError('Vendor Specific Interface Not found')

ep_out = usb.util.find_descriptor(
    intf,
    # match the first OUT endpoint
    custom_match = \
    lambda e: \
        usb.util.endpoint_direction(e.bEndpointAddress) == \
        usb.util.ENDPOINT_OUT)

assert ep_out is not None
print(ep_out)

ep_in = usb.util.find_descriptor(
    intf,
    # match the first IN endpoint
    custom_match = \
    lambda e: \
        usb.util.endpoint_direction(e.bEndpointAddress) == \
        usb.util.ENDPOINT_IN)

assert ep_in is not None
print(ep_in)

# #send in command to trigger a response
# msg = bytearray([0x03,0x12,0x13,0xFF])
# ep_out.write(msg, 100)

class PwrMonPktHdr:
    def __init__(self, data):
        if (len(data) == 12):
            self.data = struct.unpack("<LLL",data)
            self.seq      = (self.data[0] >> 16) & 0xFFFF
            self.smplSize = (self.data[0] >> 12) & 0xF
            self.numSmpl  = (self.data[0] >>  6) & 0x3F
            self.numCh    = (self.data[0] >>  0) & 0x3F
            self.startIdx = self.data[1]
            self.chBitmap = self.data[2]
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


class PwrMonPkt:
    def __init__(self, data):
        self.mv_hdr = memoryview(data)[0:12]
        self.hdr = PwrMonPktHdr(self.mv_hdr)
        self.dataLen = self.hdr.smplSize * self.hdr.numCh * self.hdr.numSmpl
        self.mv_data = memoryview(data)[12:12+self.dataLen]
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


cnt = 0
pktSize = 2212
while cnt < 1000:
    msg = bytearray()
    try:
        msg = ep_in.read(4096, 100)
    except usb.core.USBTimeoutError as err:
        pass

    pkt = PwrMonPkt(msg)
    if((cnt % 50) == 0):
        print("**********************************")
        print(pkt)

    cnt += 1

