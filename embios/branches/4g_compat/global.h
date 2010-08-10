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


#ifndef __GLOBAL_H__
#define __GLOBAL_H__


#define ICODE_ATTR __attribute__((section(".icode")))
#define ICONST_ATTR __attribute__((section(".irodata")))
#define IDATA_ATTR __attribute__((section(".idata")))
#define IBSS_ATTR __attribute__((section(".ibss")))
#define INITCODE_ATTR __attribute__((section(".initcode")))
#define INITCONST_ATTR __attribute__((section(".initrodata")))
#define INITDATA_ATTR __attribute__((section(".initdata")))
#define INITBSS_ATTR __attribute__((section(".initbss")))
#define STACK_ATTR __attribute__((section(".stack")))

#ifndef ASM_FILE
#include "errno.h"
#include "inttypes.h"
#include "math.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
typedef int bool;
#define true 1
#define false 0
#endif

#include "build/version.h"
#include "configmagic.h"
#include "debug.h"


#endif
