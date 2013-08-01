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


static struct rtc_datetime datetime;
static unsigned int batt_level;
static long next_refresh = 0;


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
    if (!next_refresh || TIMEOUT_EXPIRED(next_refresh, 10000000))
    {
        rtc_read_datetime(&datetime);
        batt_level = 22 * read_battery_mwh_current(0) / read_battery_mwh_full(0);
        next_refresh = USEC_TIMER;
    }
    snprintf(buf, sizeof(buf), "%02d:%02d", datetime.hour, datetime.minute);
    // clock
    rendertext(framebuf, 143, 4, 176, 0xffffcccc, 0, buf);
    // draw the battery meter box
    // top line
    ui->blendcolor(24, 1, 0xffffcccc, framebuf, 4, 4, 176, framebuf, 4, 4, 176);
    // bottom line
    ui->blendcolor(24, 1, 0xffffcccc, framebuf, 4, 11, 176, framebuf, 4, 11, 176);
    // left line
    ui->blendcolor(1, 6, 0xffffcccc, framebuf, 4, 5, 176, framebuf, 4, 5, 176);
    // right line
    ui->blendcolor(1, 6, 0xffffcccc, framebuf, 27, 5, 176, framebuf, 27, 5, 176);
    // tip - right
    ui->blendcolor(1, 4, 0xffffcccc, framebuf, 28, 6, 176, framebuf, 28, 6, 176);
    // remaining battery level
    ui->blendcolor(batt_level, 6, 0xc0ffcccc, framebuf, 5, 5, 176, framebuf, 5, 5, 176);
    // background of the rest space
    ui->blendcolor(22 - batt_level, 6, 0x40000000, framebuf, 5 + batt_level,
                   5, 176, framebuf, 5 + batt_level, 5, 176);
    return false;
}

void message(int x, const char* line1, const char* line2)
{
    rendertext(framebuf, x, 73, 176, 0xff3333ff, 0xa0000000, line1);
    rendertext(framebuf, x, 81, 176, 0xff3333ff, 0xa0000000, line2);
    displaylcd(0, 0, 176, 132, framebuf, 0, 0, 176);
    sleep(5000000);
}
