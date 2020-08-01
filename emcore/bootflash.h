//
//
//    Copyright 2010 TheSeven
//
//
//    This file is part of emCORE.
//
//    emCORE is free software: you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the
//    License, or (at your option) any later version.
//
//    emCORE is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with emCORE.  If not, see <http://www.gnu.org/licenses/>.
//
//


#ifndef __BOOTFLASH_H__
#define __BOOTFLASH_H__


#include "global.h"


int bootflash_filesize(const char* filename);
int bootflash_attributes(const char* filename);
void* bootflash_getaddr(const char* filename);
int bootflash_read(const char* filename, void* addr, int offset, int size);
void bootflash_readraw(void* addr, int offset, int size);
void bootflash_writeraw(void* addr, int offset, int size);
void* bootflash_getrawaddr(int offset);
bool bootflash_is_memmapped(void);


#endif
