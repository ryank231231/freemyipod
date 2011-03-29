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
#include "i2c.h"
#include "backlight.h"


void backlight_init()
{
    backlight_set_fade(10);
    backlight_set_brightness(55);
    backlight_on(true);
}

void backlight_on(bool on)
{
    i2c_sendbyte(0, 0xe6, 0x29, on ? 1 : 0);
}

void backlight_set_fade(uint8_t fade)
{
    i2c_sendbyte(0, 0xe6, 0x2b, fade);
}

void backlight_set_brightness(uint8_t brightness)
{
    i2c_sendbyte(0, 0xe6, 0x28, (63 * (brightness & 0xff)) >> 8);
}
