//
//
//    Copyright 2011 TheSeven
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


#include "emcoreapp.h"
#include "util.h"
#include "main.h"


struct emcorelib_header* loadlib(uint32_t identifier, uint32_t version, char* filename)
{
    struct emcorelib_header* lib = get_library(identifier, version, LIBSOURCE_BOOTFLASH, filename);
    if (!lib) panicf(PANIC_KILLTHREAD, "Could not load %s", filename);
    return lib;
}

void* loadpng(struct libpng_api* libpng, const char* buf, const uint32_t size,
              void* (*decoder)(struct png_info* handle))
{
    if (size == 0) panicf(PANIC_KILLTHREAD, "Could not load PNG at 0x%08X", buf);
    struct png_info* handle = libpng->png_open(buf, size);
    if (!handle) panicf(PANIC_KILLTHREAD, "Could not parse PNG at 0x%08X", buf);
    void* out = decoder(handle);
    if (!out) panicf(PANIC_KILLTHREAD, "Could not decode PNG at 0x%08X", buf);
    libpng->png_destroy(handle);
    return out;
}

bool update_display(struct chooser_data* data)
{
    char buf[6];
    struct rtc_datetime dt;
    rtc_read_datetime(&dt);
    snprintf(buf, sizeof(buf), "%02d:%02d", dt.hour, dt.minute);
    // clock
    rendertext(framebuf, 287, 4, 320, 0xffffcccc, 0, buf);
    // draw the battery meter box
    // top line
    ui->blendcolor(24, 1, 0xffffcccc, framebuf, 4, 4, 320, framebuf, 4, 4, 320);
    // bottom line
    ui->blendcolor(24, 1, 0xffffcccc, framebuf, 4, 11, 320, framebuf, 4, 11, 320);
    // left line
    ui->blendcolor(1, 6, 0xffffcccc, framebuf, 4, 5, 320, framebuf, 4, 5, 320);
    // right line
    ui->blendcolor(1, 6, 0xffffcccc, framebuf, 27, 5, 320, framebuf, 27, 5, 320);
    // tip - right
    ui->blendcolor(1, 4, 0xffffcccc, framebuf, 28, 6, 320, framebuf, 28, 6, 320);
    unsigned int batt_level = 22 * read_battery_mwh_current(0) / read_battery_mwh_full(0);
    // remaining battery level
    ui->blendcolor(batt_level, 6, 0xc0ffcccc, framebuf, 5, 5, 320, framebuf, 5, 5, 320);
    // background of the rest space
    ui->blendcolor(22 - batt_level, 6, 0x40000000, framebuf, 5 + batt_level,
                   5, 320, framebuf, 5 + batt_level, 5, 320);
    return false;
}

void message(int x, const char* line1, const char* line2)
{
    rendertext(framebuf, x, 140, 320, 0xff3333ff, 0xa0000000, line1);
    rendertext(framebuf, x, 148, 320, 0xff3333ff, 0xa0000000, line2);
    displaylcd(0, 0, 320, 240, framebuf, 0, 0, 320);
    sleep(5000000);
}
