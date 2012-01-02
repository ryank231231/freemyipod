#!/usr/bin/env python
#
#
#    Copyright 2010 TheSeven
#
#
#    This file is part of emCORE.
#
#    emCORE is free software: you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as
#    published by the Free Software Foundation, either version 2 of the
#    License, or (at your option) any later version.
#
#    emCORE is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#    See the GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with emCORE.  If not, see <http://www.gnu.org/licenses/>.
#
#


import sys
import os
import struct
import time
import hashlib
import libemcore
from libemcore import Error
import libemcoredata


def s5l8701cryptdfu(data):
    data = data.ljust((len(data) + 0x3f) & ~0x3f, b"\0")
    header = b"87011.0\0\0\x08\0\0" + struct.pack("<I", len(data))
    emcore = libemcore.Emcore()
    addr = emcore.memalign(0x10, len(data) + 0x800)
    emcore.write(addr, header.ljust(0x800, b"\0") + data)
    emcore.hmac_sha1(addr + 0x800, len(data), addr + 0x10)
    emcore.hmac_sha1(addr, 0x40, addr + 0x40)
    emcore.aesencrypt(addr, len(data) + 0x800, 1)
    data = emcore.read(addr, len(data) + 0x800)
    emcore.free(addr)
    return data


def s5l8701decryptdfu(data):
    headersize = struct.unpack("<I", data[8:12])[0]
    emcore = libemcore.Emcore()
    addr = emcore.memalign(0x10, len(data))
    emcore.write(addr, data)
    emcore.aesdecrypt(addr, len(data), 1)
    data = emcore.read(addr + headersize, len(data) - headersize)
    emcore.free(addr)
    return data


def s5l8701cryptfirmware(data):
    data = data.ljust((len(data) + 0x3f) & ~0x3f, b"\0")
    header = b"\0\0\0\0\x02\0\0\0\x01\0\0\0\x40\0\0\0\0\0\0\0" + struct.pack("<I", len(data))
    emcore = libemcore.Emcore()
    addr = emcore.memalign(0x10, len(data) + 0x800)
    emcore.write(addr, header.ljust(0x800, b"\0") + data)
    emcore.hmac_sha1(addr + 0x800, len(data), addr + 0x1c)
    emcore.hmac_sha1(addr, 0x200, addr + 0x1d4)
    emcore.aesencrypt(addr + 0x800, len(data), 1)
    data = emcore.read(addr, len(data) + 0x800)
    emcore.free(addr)
    return data


def s5l8701decryptfirmware(data):
    emcore = libemcore.Emcore()
    addr = emcore.memalign(0x10, len(data))
    emcore.write(addr, data)
    emcore.aesdecrypt(addr + 0x800, len(data) - 0x800, 1)
    data = emcore.read(addr + 0x800, len(data) - 0x800)
    emcore.free(addr)
    return data


def s5l8702cryptnor(data):
    data = data.ljust((len(data) + 0xf) & ~0xf, b"\0")
    header = b"87021.0\x01\0\0\0\0" + struct.pack("<I", len(data)) + hashlib.sha1(data).digest()[:0x10]
    emcore = libemcore.Emcore()
    addr = emcore.memalign(0x10, len(data))
    emcore.write(addr, header.ljust(0x800, b"\0") + data)
    emcore.aesencrypt(addr + 0x800, len(data), 2)
    emcore.aesencrypt(addr + 0x10, 0x10, 2)
    emcore.write(addr + 0x40, hashlib.sha1(emcore.read(addr, 0x40)).digest()[:0x10])
    emcore.aesencrypt(addr + 0x40, 0x10, 2)
    data = emcore.read(addr, len(data) + 0x800)
    emcore.free(addr)
    return data


def s5l8702decryptnor(data):
    emcore = libemcore.Emcore()
    addr = emcore.memalign(0x10, len(data))
    emcore.write(addr, data[0x800:])
    emcore.aesdecrypt(addr, len(data) - 0x800, 1)
    data = emcore.read(addr, len(data) - 0x800)
    emcore.free(addr)
    return data


def s5l8702genpwnage(data):
    cert = open(os.path.dirname(__file__) + "/libipodcrypto/s5l8702pwnage.cer", "rb").read()
    data = data.ljust(max(0x840, (len(data) + 0xf) & ~0xf), b"\0")
    header = (b"87021.0\x03\0\0\0\0" + struct.pack("<IIII", len(data) - 0x830, len(data) - 0x4f6, len(data) - 0x7b0, 0x2ba)).ljust(0x40, b"\0")
    emcore = libemcore.Emcore()
    addr = emcore.memalign(0x10, len(data))
    emcore.write(addr, header + hashlib.sha1(header).digest()[:0x10])
    emcore.aesencrypt(addr + 0x40, 0x10, 1)
    data = emcore.read(addr, 0x50) + data + cert.ljust((len(cert) + 0xf) & ~0xf, b"\0")
    emcore.free(addr)
    return data


def s5l8702genpwnage800(data):
    cert = open(os.path.dirname(__file__) + "/libipodcrypto/s5l8702pwnage800.cer", "rb").read()
    data = data.ljust(max(0x90, (len(data) + 0xf) & ~0xf), b"\0")
    header = (b"87021.0\x03\0\0\0\0" + struct.pack("<IIII", len(data) - 0x80, len(data) + 0x2ba, len(data), 0x2ba)).ljust(0x40, b"\0")
    emcore = libemcore.Emcore()
    addr = emcore.memalign(0x10, len(data))
    emcore.write(addr, header + hashlib.sha1(header).digest()[:0x10])
    emcore.aesencrypt(addr + 0x40, 0x10, 1)
    data = emcore.read(addr, 0x50).ljust(0x800, b"\0") + data + cert.ljust((len(cert) + 0xf) & ~0xf, b"\0")
    emcore.free(addr)
    return data


def s5l8720genpwnage(data):
    cert = open(os.path.dirname(__file__) + "/libipodcrypto/s5l8720pwnage.cer", "rb").read()
    data = data.ljust(max(0x640, (len(data) + 0xf) & ~0xf), b"\0")
    header = (b"87202.0\x03\0\0\0\0" + struct.pack("<IIII", len(data) - 0x630, len(data) - 0x2f2, len(data) - 0x5b0, 0x2be)).ljust(0x40, b"\0")
    emcore = libemcore.Emcore()
    addr = emcore.memalign(0x10, len(data))
    emcore.write(addr, header + hashlib.sha1(header).digest()[:0x10])
    emcore.aesencrypt(addr + 0x40, 0x10, 1)
    data = emcore.read(addr, 0x50) + data + cert.ljust((len(cert) + 0xf) & ~0xf, b"\0")
    emcore.free(addr)
    return data

def fileoperation(infilepath, outfilepath, function):
    with open(infilepath, "rb") as infile:
        infiledata = infile.read()
    
    try:
        outfiledata = function(infiledata)
    except:
        os.remove(outfilepath)
        raise
    
    with open(outfilepath, "wb") as outfile:
        outfile.write(outfiledata)

def s5l8701cryptdfufile(infile, outfile):
    fileoperation(infile, outfile, s5l8701cryptdfu)

def s5l8701decryptdfufile(infile, outfile):
    fileoperation(infile, outfile, s5l8701decryptdfu)

def s5l8701cryptfirmwarefile(infile, outfile):
    fileoperation(infile, outfile, s5l8701cryptfirmware)

def s5l8701decryptfirmwarefile(infile, outfile):
    fileoperation(infile, outfile, s5l8701decryptfirmware)

def s5l8702cryptnorfile(infile, outfile):
    fileoperation(infile, outfile, s5l8702cryptnor)

def s5l8702decryptnorfile(infile, outfile):
    fileoperation(infile, outfile, s5l8702decryptnor)

def s5l8702genpwnagefile(infile, outfile):
    fileoperation(infile, outfile, s5l8702genpwnage)

def s5l8702genpwnagefile800(infile, outfile):
    fileoperation(infile, outfile, s5l8702genpwnage800)

def s5l8720genpwnagefile(infile, outfile):
    fileoperation(infile, outfile, s5l8720genpwnage)
