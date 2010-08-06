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


#ifndef __LCDCONSOLE_H__
#define __LCDCONSOLE_H__


#include "global.h"
#include "drawing.h"
#include "lcd.h"


#define LCDCONSOLE_COLS (LCD_WIDTH / FONT_WIDTH)
#define LCDCONSOLE_ROWS (LCD_HEIGHT / FONT_HEIGHT)
#define LCDCONSOLE_OFFSETX ((LCD_WIDTH - LCDCONSOLE_COLS * FONT_WIDTH) / 2)
#define LCDCONSOLE_OFFSETY ((LCD_HEIGHT - LCDCONSOLE_ROWS * FONT_HEIGHT) / 2)


void lcdconsole_init();
void lcdconsole_putc(char string, int fgcolor, int bgcolor) ICODE_ATTR;
void lcdconsole_puts(const char* string, int fgcolor, int bgcolor) ICODE_ATTR;
void lcdconsole_putc_noblit(char string, int fgcolor, int bgcolor) ICODE_ATTR;
void lcdconsole_puts_noblit(const char* string, int fgcolor, int bgcolor) ICODE_ATTR;
void lcdconsole_update() ICODE_ATTR;


#endif
