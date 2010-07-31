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
#include "console.h"
#include "lcdconsole.h"
#include "format.h"
#include "thread.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <limits.h>


struct for_cprintf
{
    unsigned int consoles;
    size_t bytes;
};


struct mutex console_mutex;


void console_init()
{
    mutex_init(&console_mutex);
}

void cputc_internal(unsigned int consoles, char string) ICODE_ATTR;
void cputc_internal(unsigned int consoles, char string)
{
    if (consoles & 1) lcdconsole_putc(string, 0, -1);
}

static int cprfunc(void* ptr, unsigned char letter)
{
    struct for_cprintf* pr = (struct for_cprintf*)ptr;
    cputc_internal(pr->consoles, letter);
    pr->bytes++;
    return true;
}

int cprintf(unsigned int consoles, const char* fmt, ...)
{
    va_list ap;
    struct for_cprintf pr;

    pr.consoles = consoles;
    pr.bytes = 0;

    mutex_lock(&console_mutex, TIMEOUT_BLOCK);
    va_start(ap, fmt);
    format(cprfunc, &pr, fmt, ap);
    va_end(ap);
    mutex_unlock(&console_mutex);

    return pr.bytes;
}

int cvprintf(unsigned int consoles, const char* fmt, va_list ap)
{
    struct for_cprintf pr;

    pr.consoles = consoles;
    pr.bytes = 0;

    mutex_lock(&console_mutex, TIMEOUT_BLOCK);
    format(cprfunc, &pr, fmt, ap);
    mutex_unlock(&console_mutex);

    return pr.bytes;
}

void cputc(unsigned int consoles, char string)
{
    mutex_lock(&console_mutex, TIMEOUT_BLOCK);
    cputc_internal(consoles, string);
    mutex_unlock(&console_mutex);
}

void cputs(unsigned int consoles, const char* string)
{
    mutex_lock(&console_mutex, TIMEOUT_BLOCK);
    if (consoles & 1) lcdconsole_puts(string, 0, -1);
    mutex_unlock(&console_mutex);
}

void cflush(unsigned int consoles)
{
    mutex_lock(&console_mutex, TIMEOUT_BLOCK);
    if (consoles & 1) lcdconsole_update();
    mutex_unlock(&console_mutex);
}
