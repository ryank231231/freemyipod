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


#ifndef __PANIC_H__
#define __PANIC_H__


#define __need___va_list
#include <stdarg.h>
#include "global.h"


int vsnprintf (char *buf, size_t size, const char *fmt, __VALIST ap);

int snprintf (char *buf, size_t size, const char *fmt, ...)
              ATTRIBUTE_PRINTF(3, 4);


#endif
