#! /usr/bin/python

"""
Modified by MartianZ
2014-10-16

Rewrite for PyUSB 1.0+ Version

"""

"""
dfuse.py - DFU (Device Firmware Upgrade) tool for STM32 Processor.
"SE" in DfuSe stands for "STmicroelectronics Extention".

Copyright (C) 2010, 2011 Free Software Initiative of Japan
Author: NIIBE Yutaka <gniibe@fsij.org>

This file is a part of Gnuk, a GnuPG USB Token implementation.

Gnuk is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Gnuk is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""


from intel_hex import *
import sys, time, struct

# INPUT: intel hex file

# As of October 2010 (DfuSe V3.0.1 - 06/18/2010), it seems that
# following features are not supported by DfuSe implementation on
# target:
#
#     unprotect
#     leave_dfu_mode
#     write to option bytes
#     erase for mass erase


# See: AN3156 by STMicroelectronics

import usb
import usb.util as util

# check string descriptor in interrface descriptor in config descriptor: iInterface

# USB DFU class, subclass, protocol
DFU_CLASS = 0xFE
DFU_SUBCLASS = 0x01
DFU_STM32PROTOCOL_0 = 0
DFU_STM32PROTOCOL_2 = 2

# DFU request
DFU_DETACH    = 0x00
DFU_DNLOAD    = 0x01
DFU_UPLOAD    = 0x02
DFU_GETSTATUS = 0x03
DFU_CLRSTATUS = 0x04
DFU_GETSTATE  = 0x05
DFU_ABORT     = 0x06

# DFU status
DFU_STATUS_OK                 = 0x00
DFU_STATUS_ERROR_TARGET       = 0x01
DFU_STATUS_ERROR_FILE         = 0x02
DFU_STATUS_ERROR_WRITE        = 0x03
DFU_STATUS_ERROR_ERASE        = 0x04
DFU_STATUS_ERROR_CHECK_ERASED = 0x05
DFU_STATUS_ERROR_PROG         = 0x06
DFU_STATUS_ERROR_VERIFY       = 0x07
DFU_STATUS_ERROR_ADDRESS      = 0x08
DFU_STATUS_ERROR_NOTDONE      = 0x09
DFU_STATUS_ERROR_FIRMWARE     = 0x0a
DFU_STATUS_ERROR_VENDOR       = 0x0b
DFU_STATUS_ERROR_USBR         = 0x0c
DFU_STATUS_ERROR_POR          = 0x0d
DFU_STATUS_ERROR_UNKNOWN      = 0x0e
DFU_STATUS_ERROR_STALLEDPKT   = 0x0f

# DFU state
STATE_APP_IDLE                = 0x00
STATE_APP_DETACH              = 0x01
STATE_DFU_IDLE                = 0x02
STATE_DFU_DOWNLOAD_SYNC       = 0x03
STATE_DFU_DOWNLOAD_BUSY       = 0x04
STATE_DFU_DOWNLOAD_IDLE       = 0x05
STATE_DFU_MANIFEST_SYNC       = 0x06
STATE_DFU_MANIFEST            = 0x07
STATE_DFU_MANIFEST_WAIT_RESET = 0x08
STATE_DFU_UPLOAD_IDLE         = 0x09
STATE_DFU_ERROR               = 0x0a

# Return tuple of 4-bytes from integer
def get_four_bytes (v):
    return [ v % 256, (v >> 8)%256, (v >> 16)%256, (v >> 24) ]

class DFU_STM32(object):
    def __init__(self, device, configuration, interface):
        """
        __init__(device, configuration, interface) -> None
        Initialize the device.
        device: usb.Device object.
        configuration: configuration number.
        interface: usb.Interface object representing the interface and altenate setting.
        """
        if interface.bInterfaceClass != DFU_CLASS:
            raise ValueError, "Wrong interface class"
        if interface.bInterfaceSubClass != DFU_SUBCLASS:
            raise ValueError, "Wrong interface sub class"

        self.__protocol = interface.bInterfaceProtocol
        



        #self.__devhandle = device.open()
        #if self.__devhandle is None:
        #    raise ValueError, "Error: Unable to open device!"
        

        #self.__devhandle.setConfiguration(configuration)
        #self.__devhandle.claimInterface(interface)
        #self.__devhandle.setAltInterface(0)
        self.__dev = device
        self.__intf = interface.bInterfaceNumber
        self.__alt = interface.bAlternateSetting
        #self.__conf = configuration
        # Initialize members
        self.__blocknum = 0

    def ll_getdev(self):
        return self.__devhandle

    def ll_get_status(self):
        # Status, PollTimeout[3], State, String
        return self.__dev.ctrl_transfer(0xa1,
                                           DFU_GETSTATUS,
                                           wValue = 0,
                                           wIndex = self.__intf,
                                           data_or_wLength = 6,
                                           timeout = 3000000)

    def ll_clear_status(self):
        return self.__dev.ctrl_transfer(0x21,
                                           DFU_CLRSTATUS,
                                           wValue = 0,
                                           wIndex = self.__intf,
                                           data_or_wLength = None)

    # Upload: TARGET -> HOST
    def ll_upload_block(self, block_num):
        return self.__dev.ctrl_transfer(0xa1,
                                           DFU_UPLOAD,
                                           wValue = block_num,
                                           wIndex = self.__intf,
                                           data_or_wLength = 1024,
                                           timeout = 3000000)

    # Download: HOST -> TARGET
    def ll_download_block(self, block_num, block):
        return self.__dev.ctrl_transfer(0x21,
                                           DFU_DNLOAD,
                                           wValue = block_num,
                                           wIndex = self.__intf,
                                           data_or_wLength = block)

    def dfuse_read_memory(self):
        blocknum = self.__blocknum
        self.__blocknum = self.__blocknum + 1
        try:
            block = self.ll_upload_block(blocknum)
            return block
        except:
            s = self.ll_get_status()
            while s[4] == STATE_DFU_DOWNLOAD_BUSY:
                time.sleep(0.1)
                s = self.ll_get_status()
            raise ValueError, "Read memory failed (%d)" % s[0]

    def dfuse_set_address_pointer(self, address):
        bytes = get_four_bytes (address)
        self.__blocknum = 2
        self.ll_download_block(0, [0x21] + bytes)
        s = self.ll_get_status()
        while s[4] == STATE_DFU_DOWNLOAD_BUSY:
            time.sleep(0.1)
            s = self.ll_get_status()
        if s[4] != STATE_DFU_DOWNLOAD_IDLE:
            raise ValueError, "Set Address Pointer failed"

    def dfuse_erase(self, address):
        bytes = get_four_bytes (address)
        self.ll_download_block(0, [0x41] + bytes)
        s = self.ll_get_status()
        while s[4] == STATE_DFU_DOWNLOAD_BUSY:
            time.sleep(0.1)
            s = self.ll_get_status()
        if s[4] != STATE_DFU_DOWNLOAD_IDLE:
            raise ValueError, "Erase failed"

    def dfuse_write_memory(self, block):
        blocknum = self.__blocknum
        self.__blocknum = self.__blocknum + 1
        self.ll_download_block(blocknum, block)
        s = self.ll_get_status()
        while s[4] == STATE_DFU_DOWNLOAD_BUSY:
            time.sleep(0.1)
            s = self.ll_get_status()
        if s[4] != STATE_DFU_DOWNLOAD_IDLE:
            raise ValueError, "Write memory failed"

    def download(self, ih):
        # First, erase pages
        sys.stdout.write("Erasing: ")
        sys.stdout.flush()
        last_addr = 0
        for start_addr in sorted(ih.memory.keys()):
            data = ih.memory[start_addr]
            end_addr = start_addr + len(data)
            addr = start_addr & 0xfffffc00
            if not last_addr == 0:
                i = 0
                if last_addr > addr:
                    addr = last_addr
                else:
                    while last_addr < addr:
                        self.dfuse_erase(last_addr)
                        if i & 0x03 == 0x03:
                            sys.stdout.write(".")
                            sys.stdout.flush()
                            last_addr += 1024
                        i += 1
            i = 0
            while addr < end_addr:
                self.dfuse_erase(addr)
                if i & 0x03 == 0x03:
                    sys.stdout.write("#")
                    sys.stdout.flush()
                addr += 1024
                i += 1
            last_addr = addr
        sys.stdout.write("\n")
        sys.stdout.flush()
        # Then, write pages
        sys.stdout.write("Writing: ")
        sys.stdout.flush()
        last_addr = 0
        for start_addr in sorted(ih.memory.keys()):
            data = ih.memory[start_addr]
            end_addr = start_addr + len(data)
            addr = start_addr & 0xfffffc00
            if not last_addr == 0:
                i = 0
                while last_addr < addr:
                    if i & 0x03 == 0x03:
                        sys.stdout.write(".")
                        sys.stdout.flush()
                    last_addr += 1024
                    i += 1
            i = 0
            if addr != start_addr:
                self.dfuse_set_address_pointer(start_addr)
                self.dfuse_write_memory(data[0:(addr + 1024 - start_addr)])
                data = data[(addr + 1024 - start_addr):]
                addr += 1024
            self.dfuse_set_address_pointer(addr)
            while addr < end_addr:
                self.dfuse_write_memory(data[i*1024:(i+1)*1024])
                if i & 0x03 == 0x03:
                    sys.stdout.write("#")
                    sys.stdout.flush()
                addr += 1024
                i += 1
            last_addr = addr
        if self.__protocol == DFU_STM32PROTOCOL_0:
            # 0-length write at the end
            self.ll_download_block(self.__blocknum, None)
            s = self.ll_get_status()
            if s[4] == STATE_DFU_MANIFEST:
                time.sleep(1)
                try:
                    s = self.ll_get_status()
                except:
                    self.__devhandle.reset()
            elif s[4] == STATE_DFU_MANIFEST_WAIT_RESET:
                self.__devhandle.reset()
            elif s[4] != STATE_DFU_IDLE:
                raise ValueError, "write failed (%d)." % s[4]
        else:
            self.ll_clear_status()
            self.ll_clear_status()
        sys.stdout.write("\n")
        sys.stdout.flush()

    def verify(self, ih):
        s = self.ll_get_status()
        if s[4] != STATE_DFU_IDLE:
            self.ll_clear_status()
        # Read pages
        sys.stdout.write("Reading: ")
        sys.stdout.flush()
        last_addr = 0
        for start_addr in sorted(ih.memory.keys()):
            data = ih.memory[start_addr]
            end_addr = start_addr + len(data)
            addr = start_addr & 0xfffffc00
            if not last_addr == 0:
                i = 0
                while last_addr < addr:
                    if i & 0x03 == 0x03:
                        sys.stdout.write(".")
                        sys.stdout.flush()
                    last_addr += 1024
                    i += 1
            if addr != start_addr:
                self.dfuse_set_address_pointer(addr)
                self.ll_clear_status()
                self.ll_clear_status()
                block = self.dfuse_read_memory()
                j = 0
                for c in data[0:(addr + 1024 - start_addr)]:
                    if (ord(c)&0xff) != block[j + start_addr - addr]:
                        raise ValueError, "verify failed at %08x" % (addr + i*1024+j)
                    j += 1
                data = data[(addr + 1024 - start_addr):]
                addr += 1024
                self.ll_clear_status()
                self.ll_clear_status()
            self.dfuse_set_address_pointer(addr)
            self.ll_clear_status()
            self.ll_clear_status()
            i = 0
            while addr < end_addr:
                block = self.dfuse_read_memory()
                j = 0
                for c in data[i*1024:(i+1)*1024]:
                    if (ord(c)&0xff) != block[j]:
                        raise ValueError, "verify failed at %08x" % (addr + i*1024+j)
                    j += 1
                if i & 0x03 == 0x03:
                    sys.stdout.write("#")
                    sys.stdout.flush()
                addr += 1024
                i += 1
            last_addr = addr
            self.ll_clear_status()
            self.ll_clear_status()
        self.ll_clear_status()
        sys.stdout.write("\n")
        sys.stdout.flush()
