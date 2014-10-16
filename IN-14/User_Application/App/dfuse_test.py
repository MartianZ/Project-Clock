#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import hid
from PyQt4 import QtGui, QtCore, uic
import time
import usb
import usb.core
import usb.util
import usb.backend.libusb1
from dfuse import *
from intel_hex import *

b = usb.backend.libusb1.get_backend()
dev = usb.core.find(backend=b, idVendor=0x0001, idProduct=0xdf11)
dev.set_configuration()

#dev, config, intf = self.get_dfu_device()
#print "Device:", dev.filename
#print "Configuration", config.value
#print "Interface", intf.interfaceNumber
print dev
cfg = dev.get_active_configuration()
intf = cfg[(0,0)]
print intf.bInterfaceClass

dfu = DFU_STM32(dev, cfg, intf)


s = dfu.ll_get_status()
if s[4] == STATE_DFU_ERROR:
    dfu.ll_clear_status()
s = dfu.ll_get_status()
print s
if s[4] == STATE_DFU_IDLE:
    exit
transfer_size = 1024
if s[0] != DFU_STATUS_OK:
    print s
    exit

ih = intel_hex(r'/Users/MartianZ/Documents/NixieTube-Clock/IN-14/STM32_C8_NixieTube/main.hex')
dfu.download(ih)
dfu.verify(ih)