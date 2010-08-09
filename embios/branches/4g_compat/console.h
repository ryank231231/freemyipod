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


#ifndef __CONSOLE_H__
#define __CONSOLE_H__


#include "global.h"
#include <stdarg.h>


void cputc(unsigned int consoles, char string) ICODE_ATTR;
void cputc_noblit(unsigned int consoles, char string) ICODE_ATTR;
void cputs(unsigned int consoles, const char* string) ICODE_ATTR;
void cputs_noblit(unsigned int consoles, const char* string) ICODE_ATTR;
int cprintf(unsigned int consoles, const char* fmt, ...) ICODE_ATTR;
int cvprintf(unsigned int consoles, const char* fmt, va_list ap) ICODE_ATTR;
void cflush(unsigned int consoles) ICODE_ATTR;


#endif
