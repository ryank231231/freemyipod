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


#include "global.h"
#include "hmacsha1.h"
#include "s5l8701.h"
#include "thread.h"


struct mutex hmacsha1_mutex;


void hmacsha1(void* data, uint32_t size, void* result)
{
    uint32_t ptr, i;
    uint32_t ctrl = 2;
    mutex_lock(&hmacsha1_mutex, TIMEOUT_BLOCK);
    PWRCON(1) &= ~4;
    for (ptr = 0; ptr < (size >> 2); ptr += 0x10)
    {
      for (i = 0; i < 0x10; i++) HASHDATAIN[i] = ((uint32_t*)data)[ptr + i];
      HASHCTRL = ctrl;
      ctrl = 0xA;
      while (HASHCTRL & 1) yield();
    }
    for (i = 0; i < 5; i ++) ((uint32_t*)result)[i] = HASHRESULT[i];
    PWRCON(1) |= 4;
    mutex_unlock(&hmacsha1_mutex);
}
