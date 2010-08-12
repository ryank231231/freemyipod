//
//
//    Copyright 2010 TheSeven
//
//
//    This file is part of emBIOS.
//
//    emBIOS is free software: you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the
//    License, or (at your option) any later version.
//
//    emBIOS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with emBIOS.  If not, see <http://www.gnu.org/licenses/>.
//
//


#include "global.h"
#include "syscallapi.h"
#include "panic.h"
#include "console.h"
#include "dir.h"
#include "file.h"
#include "format.h"
#include "lcdconsole.h"
#include "storage.h"
#include "shutdown.h"
#include "thread.h"
#include "ucl.h"
#include "bootflash.h"
#include "timer.h"
#include "i2c.h"
#include "interrupt.h"
#include "lcd.h"
#include "mmu.h"
#include "nand.h"
#include "power.h"
#include "execimage.h"
#include "libc/include/string.h"
#include "libc/include/stdlib.h"
#include "libc/include/stdio.h"


struct embios_syscall_table syscall_table ICONST_ATTR =
{
    .table_version = EMBIOS_API_VERSION,
    .table_minversion = EMBIOS_MIN_API_VERSION,
	.panic = panic,
    .panicf = panicf,
    .cprintf = cprintf,
    .cvprintf = cvprintf,
    .cputc = cputc,
    .cputs = cputs,
    .cwrite = cwrite,
    .cflush = cflush,
    .cgetc = cgetc,
    .cread = cread,
    .creada = creada,
    .format = format,
    .vuprintf = vuprintf,
    .shutdown = shutdown,
    .strcasecmp = strcasecmp,
    .strncasecmp = strncasecmp,
    .strcasestr = strcasestr,
    .strlcat = strlcat,
    .strlcpy = strlcpy,
    .mutex_init = mutex_init,
    .mutex_lock = mutex_lock,
    .mutex_unlock = mutex_unlock,
    .wakeup_init = wakeup_init,
    .wakeup_wait = wakeup_wait,
    .wakeup_signal = wakeup_signal,
    .sleep = sleep,
    .thread_create = thread_create,
    .thread_exit = thread_exit,
    .thread_suspend = thread_suspend,
    .thread_resume = thread_resume,
    .thread_terminate = thread_terminate,
    .__errno = __errno,
    .ucl_decompress = ucl_decompress,
    .read_native_timer = read_native_timer,
    .read_usec_timer = read_usec_timer,
    .i2c_send = i2c_send,
    .i2c_recv = i2c_recv,
    .i2c_sendbyte = i2c_sendbyte,
    .i2c_recvbyte = i2c_recvbyte,
    .interrupt_enable = interrupt_enable,
    .interrupt_set_handler = interrupt_set_handler,
    .int_timer_set_handler = int_timer_set_handler,
    .clean_dcache = clean_dcache,
    .invalidate_dcache = invalidate_dcache,
    .invalidate_icache = invalidate_icache,
    .power_off = power_off,
    .charging_state = charging_state,
    .atoi = atoi,
    .memchr = memchr,
    .memcmp = memcmp,
    .memcpy = memcpy,
    .memmove = memmove,
    .memset = memset,
    .qsort = qsort,
    .srand = srand,
    .rand = rand,
    .snprintf = snprintf,
    .vsnprintf = vsnprintf,
    .sscanf = sscanf,
    .strcat = strcat,
    .strchr = strchr,
    .strcmp = strcmp,
    .strcpy = strcpy,
    .strlen = strlen,
    .strncmp = strncmp,
    .strrchr = strrchr,
    .strstr = strstr,
    .strtok_r = strtok_r,
#ifdef HAVE_STORAGE
    .opendir = opendir,
    .closedir = closedir,
    .readdir = readdir,
    .mkdir = mkdir,
    .rmdir = rmdir,
    .renderbmp = renderbmp,
    .renderchar = renderchar,
    .execimage = execimage,
    .ftruncate = ftruncate,
    .fsync = fsync,
    .close = close,
    .write = write,
    .read = read,
    .lseek = lseek,
    .remove = remove,
    .file_open = file_open,
    .rename = rename,
    .file_creat = file_creat,
    .filesize = filesize,
    .storage_read_sectors_md = storage_read_sectors_md,
    .storage_write_sectors_md = storage_write_sectors_md,
#endif
#ifdef HAVE_LCD
    .lcdconsole_putc_noblit = lcdconsole_putc_noblit,
    .lcdconsole_puts_noblit = lcdconsole_puts_noblit,
    .lcdconsole_write_noblit = lcdconsole_write_noblit,
    .lcdconsole_update = lcdconsole_update,
    .lcdconsole_putc = lcdconsole_putc,
    .lcdconsole_puts = lcdconsole_puts,
    .lcdconsole_write = lcdconsole_write,
    .displaylcd = displaylcd,
    .displaylcd_sync = displaylcd_sync,
    .displaylcd_busy = displaylcd_busy,
    .displaylcd_safe = displaylcd_safe,
    .lcd_get_width = lcd_get_width,
    .lcd_get_height = lcd_get_height,
    .lcd_get_bytes_per_pixel = lcd_get_bytes_per_pixel,
#endif
#ifdef HAVE_BOOTFLASH
    .bootflash_filesize = bootflash_filesize,
    .bootflash_attributes = bootflash_attributes,
    .bootflash_getaddr = bootflash_getaddr,
    .bootflash_read = bootflash_read,
    .bootflash_readraw = bootflash_readraw,
    .bootflash_writeraw = bootflash_writeraw,
    .bootflash_getrawaddr = bootflash_getrawaddr,
#endif
#ifdef HAVE_NAND
    .nand_read_page = nand_read_page,
    .nand_block_erase = nand_block_erase,
    .nand_read_page_fast = nand_read_page_fast,
    .nand_write_page = nand_write_page,
    .nand_write_page_start = nand_write_page_start,
    .nand_write_page_collect = nand_write_page_collect,
    .nand_get_device_type = nand_get_device_type,
#endif
};
