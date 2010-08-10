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
#include "lcdconsole.h"
#include "console.h"
#include "accel.h"

static const char welcomestring[] INITCONST_ATTR = "emBIOS v" VERSION "\n";

#include "s5l8720.h"

void init() INITCODE_ATTR;
void init()
{
    scheduler_init();
    lcdconsole_init();
    cputs(1, welcomestring);
    while(1)
    {
        cprintf(1, "TBCNT: %d \n", TBCNT);
    }
  
}
