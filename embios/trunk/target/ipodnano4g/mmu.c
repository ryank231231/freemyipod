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
#include "mmu.h"


void clean_dcache()
{
    asm volatile(
        "clean_dcache_loop:        \n\t"
        "MRC p15, 0, R15,c7,c10,3  \n\t"
        "BNE clean_dcache_loop     \n\t"
        "MOV R0, #0                \n\t"
        "MCR p15, 0, R0,c7,c10,4   \n\t"
        "MOV PC, LR                \n\t"
    );
}

void invalidate_dcache()
{
    asm volatile(
        "invalidate_dcache_loop:   \n\t"
        "MRC p15, 0, R15,c7,c14,3  \n\t"
        "BNE invalidate_dcache_loop\n\t"
        "MOV R0, #0                \n\t"
        "MCR p15, 0, R0,c7,c10,4   \n\t"
        "MOV PC, LR                \n\t"
    );
}

void invalidate_icache()
{
    asm volatile(
        "MOV R0, #0                \n\t"
        "MCR p15, 0, R0,c7,c5,0    \n\t"
        "MOV PC, LR                \n\t"
    );
}
