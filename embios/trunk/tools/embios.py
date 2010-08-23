#!/usr/bin/env python
#
#
#    Copyright 2010 TheSeven, benedikt93, Farthen
#
#
#    This file is part of emBIOS.
#
#    emBIOS is free software: you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as
#    published by the Free Software Foundation, either version 2 of the
#    License, or (at your option) any later version.
#
#    emBIOS is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#    See the GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with emBIOS.  If not, see <http://www.gnu.org/licenses/>.
#
#

import sys
import os
import inspect
import re

import libembios
from libembios import Error
import libembiosdata

class NotImplementedError(Error):
    pass

class ArgumentError(Error):
    pass

class ArgumentTypeError(Error):
    def __init__(self, expected, seen=False):
        self.expected = expected
        self.seen = seen
    def __str__(self):
        if self.seen:
            return "Expected " + str(self.expected) + " but saw " + str(self.seen)
        else:
            return "Expected " + str(self.expected) + ", but saw something else"


def usage(errormsg=None, specific=False):
    """
        Prints the usage information.
        It is auto generated from various places.
    """
    logger = Logger()
    cmddict= Commandline.cmddict
    doc = {}
    # This sorts the output of various internal functions
    # and puts everything in easy readable form
    for function in cmddict:
        function = cmddict[function].func
        docinfo = {}
        name = function.__name__
        args = inspect.getargspec(function)[0]
        docinfo['varargs'] = False
        if inspect.getargspec(function)[1]:
            docinfo['varargs'] = True
        kwargvalues = inspect.getargspec(function)[3]
        kwargs = {}
        if args:
            if kwargvalues:
                argnum = len(args) - len(kwargvalues)
                kwargnum = len(kwargvalues)
                kwargs = dict(zip(args[argnum:], kwargvalues))
            else:
                argnum = len(args)
        else:
            argnum = 0
        docinfo['args'] = args[1:argnum]
        docinfo['kwargs'] = kwargs
        if function.__doc__:
            # strip unneccessary whitespace
            docinfo['documentation'] = re.sub(r'\n        ', '\n', function.__doc__)
        else:
            docinfo['documentation'] = None
        doc[name] = docinfo

    if not specific:
        logger.log("Please provide a command and (if needed) parameters as command line arguments\n\n")
        logger.log("Available commands:\n\n")
    else:
        logger.log("\n")
    for function in sorted(doc.items()):
        function = function[0]
        if specific == False or specific == function:
            logger.log("  " + function + " ")
            for arg in doc[function]['args']:
                logger.log("<" + arg + "> ")
            if doc[function]['kwargs']:
                for kwarg in doc[function]['kwargs']:
                    logger.log("[" + kwarg + "] ")
            if doc[function]['varargs']:
                logger.log("<db1> ... <dbN>")
            if doc[function]['documentation']:
                logger.log(doc[function]['documentation']+"\n")
    
    logger.log("\n")

    if errormsg:
        logger.error(str(errormsg)+"\n")
    exit(2)


class Logger(object):
    """
        Simple stdout logger.
        Loglevel 4 is most verbose, Loglevel 0 only say something if there is an error.
    """
    def __init__(self):
        # Possible values: 0 (only errors), 1 (warnings), 2 (info, recommended for production use), 3 and more (debug)
        self.loglevel = 3
        
    def log(self, text):
        sys.stdout.write(text)
    
    def debug(self, text):
        if self.loglevel >= 3:
            self.log(text)
    
    def info(self, text):
        if self.loglevel >= 2:
            self.log(text)
    
    def warning(self, text):
        if self.loglevel >= 1:
            self.log("WARNING: " + text)
    
    def error(self, text):
        self.log("ERROR: " + text)


def command(func):
    """
        Decorator for all commands.
        The decorated function is called with (self, all, other, arguments, ...)
    """
    def decorator(*args):
        return func(args[0], *args[1:])
    func._command = True
    decorator.func = func
    return decorator


def commandClass(cls):
    """
        Decorator for the class. Sets the self.cmddict of the class
        to all functions decorated with @command
    """
    cls.cmddict = {}
    for attr, value in cls.__dict__.iteritems():
        if getattr(value, 'func', False):
            if getattr(value.func, '_command', False):
                cls.cmddict[value.func.__name__] = value
    return cls


@commandClass
class Commandline(object):
    """
        If you want to create a new commandline function you just need to
        create a function with the name of it in this class and decorate
        it with the decorator @command. If you don't want to call the desired
        function (wrong arguments etc) just raise ArgumentError with or
        without an error message or raise ArgumentCountError
    """
    def __init__(self):
        self.logger = Logger()
        try:
            self.embios = libembios.Embios()
        except libembios.DeviceNotFoundError:
            self.logger.error("No emBIOS device found!")
            end(1)
        try:
            self.getinfo("version")
        except libembios.DeviceNotFoundError:
                self.logger.error("Device not found!")
                exit(2)
        
    def _parsecommand(self, func, args):
        # adds self to the commandline args.
        # this is needed because the functions need access to their class.
        args.insert(0, self)
        if func in self.cmddict:
            try:
                self.cmddict[func](*args)
            except ArgumentError, e:
                usage(e)
            except ArgumentError:
                usage("Syntax Error in function '" + func + "'")
            except ArgumentTypeError, e:
                usage(e)
            except NotImplementedError:
                self.logger.error("This function is not implemented yet!")
            except libembios.DeviceError, e:
                self.logger.error(str(e))
            except TypeError, e:
                if str(e).split(" ", 1)[0] == func + "()":
                    self.logger.error(usage("Argument Error in '" + func + "': Wrong argument count", specific=func))
                else:
                    raise
        else:
            usage("No such command")
    
    @staticmethod
    def _bool(something):
        """
            Converts quite everything into bool.
        """
        if type(something) == bool:
            return something
        elif type(something) == int or type(something) == long:
            return bool(something)
        elif type(something == str):
            truelist = ['true', '1', 't', 'y', 'yes']
            falselist = ['false', '0', 'f', 'n', 'no']
            if something.lower() in truelist:
                return True
            elif something.lower() in falselist:
                return False
        raise ArgumentTypeError("bool", "'"+str(something)+"'")

    @staticmethod
    def _hexint(something):
        """
            Converts quite everything to a hexadecimal represented integer.
            This works for default arguments too, because it returns
            None when it found that it got a NoneType object.
        """
        if type(something) == int or type(something) == long:
            return something
        elif type(something) == str:
            try:
                return int(something, 16)
            except ValueError:
                raise ArgumentTypeError("hexadecimal coded integer", "'"+str(something)+"'")
        elif type(something) == NoneType:
            return None
        else:
            raise ArgumentTypeError("hexadecimal coded integer", "'"+str(something)+"'")

    @staticmethod
    def _strcheck(string, values):
        if string in values:
            return string
        else:
            expected = ""
            for item in values:
                expected += "'" + item + "', "
            expected = expected[:-2]
            raise ArgumentTypeError("one out of " + expected, "'" + string + "'")
    
    @staticmethod
    def _hex(integer):
        return "0x%x" % integer
    
    @command
    def getinfo(self, infotype):
        """
            Get info on the running emBIOS.
            <infotype> may be either of 'version', 'packetsize', 'usermemrange'.
        """
        if infotype == "version":
            resp = self.embios.getversioninfo()
            self.logger.info("Connected to "+libembiosdata.swtypes[resp.swtypeid] + " v" + str(resp.majorv) + "." + str(resp.minorv) +
                             "." + str(resp.patchv) + " r" + str(resp.revision) + " running on " + libembiosdata.hwtypes[resp.hwtypeid] + "\n")
        elif infotype == "packetsize":
            resp = self.embios.getpacketsizeinfo()
            self.logger.info("Maximum packet sizes: "+str(resp))
        elif infotype == "usermemrange":
            resp = self.embios.getusermemrange()
            self.logger.info("The user memory range is "+self._hex(resp.lower)+" - "+self._hex(resp.upper-1))
        else:
            raise ArgumentTypeError("one out of 'version', 'packetsize', 'usermemrange'", infotype)
    
    @command
    def reset(self, force=False):
        """
            Resets the device"
            If <force> is 1, the reset will be forced, otherwise it will be gracefully,
            which may take some time.
        """
        force = self._bool(force)
        if force: self.logger.info("Resetting forcefully...\n")
        else: self.logger.info("Resetting...\n")
        self.embios.reset(force)
    
    @command
    def poweroff(self, force=False):
        """
            Powers the device off
            If <force> is 1, the poweroff will be forced, otherwise it will be gracefully,
            which may take some time.
        """
        force = self._bool(force)
        if force: self.logger.info("Resetting forcefully...\n")
        else: self.logger.info("Resetting...\n")
        self.embios.reset(force)
    
    @command
    def uploadfile(self, addr, filename):
        """
            Uploads a file to the device
            <offset>: the address to upload the file to
            <filename>: the path to the file
        """
        addr = self._hexint(addr)
        try:
            f = open(filename, 'rb')
        except IOError:
            raise ArgumentError("File not readable. Does it exist?")
        self.logger.info("Writing file '"+filename+"' to memory at "+self._hex(addr)+"...")
        with f:
            self.embios.write(addr, f.read())
        self.logger.info("done\n")
        
        

    @command
    def downloadfile(self, addr, size, filename):
        """
            Uploads a file to the device
            <offset>: the address to upload the file to
            <size>: the number of bytes to be read
            <filename>: the path to the file
        """
        addr = self._hexint(addr)
        size = self._hexint(size)
        try:
            f = open(filename, 'wb')
        except IOError:
            raise ArgumentError("Can not open file for write!")
        self.logger.info("Reading data from address "+self._hex(addr)+" with the size "+self._hex(size)+" to '"+filename+"'...")
        with f:
            f.write(self.embios.read(addr, size))
        self.logger.info("done\n")

    @command
    def uploadint(self, addr, integer):
        """
            Uploads a single integer to the device
            <offset>: the address to upload the integer to
            <data>: the integer to upload
        """
        addr = self._hexint(addr)
        integer = self._hexint(integer)
        if integer > 0xFFFFFFFF:
            raise ArgumentError("Specified integer too long")
        data = chr(integer)
        self.embios.writemem(addr, data)
        self.logger.info("Integer '"+self._hex(integer)+"' written successfully to "+self._hex(addr))

    @command
    def downloadint(self, addr):
        """
            Downloads a single integer from the device and prints it to the console window
            <offset>: the address to download the integer from
        """
        addr = self._hexint(addr)
        data = self.embios.readmem(addr, 1)
        integer = ord(data)
        self.logger.info("Integer '"+self._hex(integer)+"' read from address "+self._hex(addr))

    @command
    def i2crecv(self, bus, slave, addr, size):
        """
            Reads data from an I2C device
            <bus> the bus index
            <slave> the slave address
            <addr> the start address on the I2C device
            <size> the number of bytes to read
        """
        bus = self._hexint(bus)
        slave = self._hexint(slave)
        addr = self._hexint(addr)
        size = self._hexint(size)
        raise NotImplementedError

    @command
    def i2csend(self, bus, slave, addr, *args):
        """
            Writes data to an I2C device
            <bus> the bus index
            <slave> the slave address
            <addr> the start address on the I2C device
            <db1> ... <dbN> the data in single bytes, seperated by whitespaces,
                eg. 0x37 0x56 0x45 0x12
        """
        bus = self._hexint(bus)
        slave = self._hexint(slave)
        addr = self._hexint(addr)
        data = []
        for arg in args:
            data.append(self._hexint(arg))
        raise NotImplementedError

    @command
    def readusbconsole(self, size, outtype):
        """
            Reads data from the USB console.
            <size>: the number of bytes to read
            <outtype>: defines how to output the result:
                'file': writes the result to file <file>
                'printstring': writes the result as string to the console window
                'printhex': writes the result in hexedit notation to the console window
            <file>: the file to write the result to, can be omitted
                if <outtype> is not 'file'
        """
        size = self._hexint(size)
        raise NotImplementedError
        

    @command
    def writeusbconsole_file(self, file, offset=0, length=None):
        """
            Writes the file <file> to the USB console.
            Optional params <offset> <length>: specify the range in <file> to write
        """
        
        offset = self._hexint(offset)
        length = self._hexint(length)
        raise NotImplementedError

    @command
    def writeusbconsole_direct(self, *args):
        """
            Writes the strings <db1> ... <dbN> to the USB console."
        """
        raise NotImplementedError

    @command
    def readdevconsole(self, bitmask, size, outtype, file=None):
        """
            Reads data from one or more of the device's consoles.
            <bitmask>: the bitmask of the consoles to read from
            <size>: the number of bytes to read
            <outtype>: defines how to output the result:
                'file': writes the result to file <file>
                'printstring': writes the result as string to the console window
                'printhex': writes the result in hexedit notation to the console window
            <file>: the file to write the result to, can be omitted
                if <outtype> is not 'file'
        """
        bitmask = self._hexint(bitmask)
        size = self._hexint(size)
        outtype = self._strcheck(['file', 'printstring', 'printhex'])
        raise NotImplementedError

    @command
    def writedevconsole_file(self, bitmask, file, offset=0, length=None):
        """
            Writes the file <file> to the device consoles specified by <bitmask>
            Optional params <offset> <length>: specify the range in <file> to write
        """
        bitmask = self._hexint(bitmask)
        
        offset = self._hexint(offset)
        length = self._hexint(length)
        raise NotImplementedError

    @command
    def writedevconsole_direct(self, bitmask, *args):
        """
            Writes the integers <db1> ... <dbN> to the device consoles specified
            by <bitmask>
        """
        bitmask = self._hexint(bitmask)
        data = []
        for arg in args:
            data.append(self._hexint(arg))
        raise NotImplementedError

    @command
    def flushconsolebuffers(self, bitmask):
        """
            flushes one or more of the device consoles' buffers.
            <bitmask>: the bitmask of the consoles to be flushed
        """
        bitmask = self._hexint(bitmask)
        raise NotImplementedError

    @command
    def getprocinfo(self):
        """
            Fetches data on the currently running processes
        """
        import datetime
        threads = self.embios.getprocinfo()
        self.logger.info("The device has "+str(len(threads))+" running threads:\n\n")
        for thread in threads:
            self.logger.info("  "+thread.name+":\n")
            self.logger.info("    Thread id: "+str(thread.id)+"\n")
            self.logger.info("    Thread type: "+thread.type+"\n")
            self.logger.info("    Thread state: "+thread.state+"\n")
            self.logger.info("    Priority: "+str(thread.priority)+"/256\n")
            self.logger.info("    CPU time (total): "+str(datetime.timedelta(microseconds=thread.cputime_total))+"\n")
            self.logger.info("    Stack address: "+self._hex(thread.stackaddr)+"\n")
            self.logger.info("    Registers:\n")
            for register in range(16):
                self.logger.info("      r"+str(register)+": "+self._hex(thread.regs["r"+str(register)])+"\n")
            self.logger.info("      cpsr: "+self._hex(thread.regs.cpsr))
            self.logger.info("\n")
    
    @command
    def lockscheduler(self):
        """
            Locks (freezes) the scheduler
        """
        self.embios.lockscheduler()

    @command
    def unlockscheduler(self):
        """
            Unlocks (unfreezes) the scheduler
        """
        self.embios.unlockscheduler()

    @command
    def suspendthread(self, threadid):
        """
            Suspends/resumes the thread with thread ID <threadid>
        """
        threadid = self._hexint(threadid)
        self.embios.resumethread(threadid)

    @command
    def resumethread(self, threadid):
        """
            Resumes the thread with thread ID <threadid>
        """
        threadid = self._hexint(threadid)
        self.embios.resumethread(threadid)

    @command
    def killthread(self, threadid):
        """
            Kills the thread with thread ID <threadid>
        """
        threadid = self._hexint(threadid)
        self.embios.killthread(threadid)

    @command
    def createthread(self, nameptr, entrypoint, stackptr, stacksize, threadtype, priority, state):
        """
            Creates a new thread and returns its thread ID
            <namepointer> a pointer to the thread's name
            <entrypoint> a pointer to the entrypoint of the thread
            <stackpointer> a pointer to the stack of the thread
            <stacksize> the size of the thread's stack
            <type> the thread type, vaild are: 0 => user thread, 1 => system thread
            <priority> the priority of the thread, from 1 to 255
            <state> the thread's initial state, valid are: 1 => ready, 0 => suspended
        """
        nameptr = self._hexint(nameptr)
        entrypoint = self._hexint(entrypoint)
        stackpointer = self._hexint(stackpointer)
        stacksize = self._hexint(stacksize)
        priority = self._hexint(priority)
        self.embios.createthread(nameptr, entrypoint, stackptr, stacksize, type, priority, state)
    
    @command
    def run(self, filename):
        """
            Uploads the emBIOS application <filename> to
            the beginning of the user memory and executes it
        """
        try:
            f = open(filename, "rb")
        except IOError:
            raise ArgumentError("File not readable. Does it exist?")
        data = self.embios.getusermemrange()
        addr = data.lower
        maxsize = data.upper - data.lower
        filesize = os.path.getsize(filename)
        if filesize > maxsize:
            raise ArgumentError("The file is too big, it doesn't fit into the user memory.")
        self.logger.info("Uploading application to "+self._hex(addr)+" - "+self._hex(addr+filesize)+"\n")
        self.embios.write(addr, f.read())
        self.execimage(addr)

    @command
    def execimage(self, addr):
        """
            Executes the emBIOS application at <addr>.
        """
        addr = self._hexint(addr)
        self.logger.info("Starting emBIOS app at "+self._hex(addr)+"\n")
        self.embios.execimage(addr)

    @command
    def readrawbootflash(self, addr_flash, addr_mem, size):
        """
            Reads <size> bytes from bootflash to memory.
            <addr_bootflsh>: the address in bootflash to read from
            <addr_mem>: the address in memory to copy the data to
        """
        addr_flash = self._hexint(addr_flash)
        addr_mem = self._hexint(addr_mem)
        size = self._hexint(size)
        raise NotImplementedError

    @command
    def writerawbootflash(self, addr_flash, addr_mem, size):
        """
            Writes <size> bytes from memory to bootflash.
            ATTENTION: Don't call this unless you really know what you're doing!
            This may BRICK your device (unless it has a good recovery option)
            <addr_mem>: the address in memory to copy the data from
            <addr_bootflsh>: the address in bootflash to write to
        """
        addr_flash = self._hexint(addr_flash)
        addr_mem = self._hexint(addr_mem)
        size = self._hexint(size)
        raise NotImplementedError

    @command
    def flushcaches(self):
        """
            Flushes the CPUs data and instruction caches.
        """
        self.logger.info("Flushing CPU data and instruction caches...")
        self.embios.flushcaches()
        self.logger.info("done\n")
    
    @command
    def aesencrypt(self, addr, size, keyindex):
        """
            Encrypts a buffer using a hardware key
        """
        addr = self._hexint(addr)
        size = self._hexint(size)
        keyindex = self._hexint(keyindex)
        self.embios.aesencrypt(addr, size, keyindex)
    
    @command
    def aesdecrypt(self, addr, size, keyindex):
        """
            Decrypts a buffer using a hardware key
        """
        addr = self._hexint(addr)
        size = self._hexint(size)
        keyindex = self._hexint(keyindex)
        self.embios.aesdecrypt(addr, size, keyindex)
    
    @command
    def hmac_sha1(self, addr, size, destination):
        """
            Generates a HMAC-SHA1 hash of the buffer and saves it to 'destination'
        """
        addr = self._hexint(addr)
        size = self._hexint(size)
        destination = self._hexint(destination)
        sha1size = 0x14
        self.logger.info("Generating hmac-sha1 hash from the buffer at "+self._hex(addr)+" with the size "+self._hex(size)+
                         " and saving it to "+self._hex(destination)+" - "+self._hex(destination+sha1size)+"...")
        self.embios.hmac_sha1(addr, size, destination)
        self.logger.info("done\n")
        data = self.embios.readmem(destination, sha1size)
        hash = ord(data)
        self.logger.info("The generated hash is "+self._hex(hash))

if __name__ == "__main__":
    if len(sys.argv) < 2:
        usage("No command specified")
    interface = Commandline()
    interface._parsecommand(sys.argv[1], sys.argv[2:])