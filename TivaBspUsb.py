import usb.core
import usb.util

dev = usb.core.find(idVendor=0x1cbe, idProduct=0x0007)

# was it found?
if dev is None:
    raise ValueError('Device not found')

# set the active configuration. With no arguments, the first
# configuration will be the active one
dev.set_configuration()

print(dev)

# get an endpoint instance
cfg = dev.get_active_configuration()
intf = cfg[(0,0)]

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

#send in command to trigger a response
# msg = bytearray([0x03,0x12,0x13,0xFF])
# ep_out.write(msg, 100)

# while len(msg) is not 51:
#     try:
#         msg = ep_in.read(8192, 100)
#     except usb.core.USBTimeoutError as err:
#         pass
#     else:
#         print(len(msg))
#         print(msg)

