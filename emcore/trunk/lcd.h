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


#ifndef __LCD_H__
#define __LCD_H__


#include "global.h"


#define LCD_FRAMEBUFSIZE (LCD_WIDTH * LCD_HEIGHT * LCD_BYTESPERPIXEL)


void lcd_init() INITCODE_ATTR;
void lcd_shutdown();
int lcd_get_width() ICODE_ATTR;
int lcd_get_height() ICODE_ATTR;
int lcd_get_bytes_per_pixel() ICODE_ATTR;
int lcd_get_format() ICODE_ATTR;
int lcd_translate_color(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) ICODE_ATTR;
void displaylcd_native(unsigned int startx, unsigned int endx,
                       unsigned int starty, unsigned int endy, void* data) ICODE_ATTR;
void displaylcd_safe_native(unsigned int startx, unsigned int endx,
                            unsigned int starty, unsigned int endy, void* data) ICODE_ATTR;
void filllcd_native(unsigned int startx, unsigned int endx,
                    unsigned int starty, unsigned int endy, int color) ICODE_ATTR;
void displaylcd(unsigned int x, unsigned int y, unsigned int width,
                unsigned int height, void* data, unsigned int datax,
                unsigned int datay, unsigned int stride) ICODE_ATTR;
void filllcd(unsigned int x, unsigned int y,
             unsigned int width, unsigned int height, int color) ICODE_ATTR;
bool displaylcd_busy() ICODE_ATTR;
void displaylcd_sync() ICODE_ATTR;


#endif
