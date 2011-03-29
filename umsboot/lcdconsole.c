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
#include "lcdconsole.h"
#include "drawing.h"
#include "util.h"
#include "contextswitch.h"

#define OFFSETX LCDCONSOLE_OFFSETX
#define OFFSETY LCDCONSOLE_OFFSETY
#define PIXELBYTES (LCD_BYTESPERPIXEL)
#define LINEBYTES (FRAMEBUF_WIDTH * PIXELBYTES)
#define COLBYTES (FONT_WIDTH * PIXELBYTES)
#define ROWBYTES (FONT_HEIGHT * LINEBYTES)
#define OFFSETBYTES (LINEBYTES * OFFSETY + PIXELBYTES * OFFSETX)


static unsigned char framebuf[LCD_FRAMEBUFSIZE] IBSS_ATTR;
static unsigned int current_row;
static unsigned int current_col;
static bool lcdconsole_needs_update;

void lcdconsole_init()
{
    displaylcd(0, LCD_WIDTH - 1, 0, LCD_HEIGHT - 1, (void*)0xffffffff, 0);
    memset(framebuf, LCDCONSOLE_BGCOLOR, sizeof(framebuf));
    current_row = 0;
    current_col = -1;
    lcdconsole_needs_update = false;
}

void lcdconsole_putc_noblit(char string, int fgcolor, int bgcolor)
{
    if (string == '\r') return;
    current_col++;
    if (string == '\n')
    {
        current_col = -1;
        current_row++;
        return;
    }
    if (string == '\t')
    {
        current_col |= 3;
        return;
    }
    if (current_col >= LCDCONSOLE_COLS)
    {
        current_col = 0;
        current_row++;
    }
    if (current_row >= LCDCONSOLE_ROWS)
    {
        int offset = current_row - LCDCONSOLE_ROWS + 1;
        memcpy(&framebuf[LINEBYTES * OFFSETY], &framebuf[LINEBYTES * OFFSETY + ROWBYTES * offset],
            ROWBYTES * (LCDCONSOLE_ROWS - offset));
        memset(&framebuf[LINEBYTES * OFFSETY + ROWBYTES * (LCDCONSOLE_ROWS - offset)],
            -1, ROWBYTES * offset);
        current_row = LCDCONSOLE_ROWS - 1;
    }
    renderchar(&framebuf[OFFSETBYTES + ROWBYTES * current_row + COLBYTES * current_col],
               fgcolor, bgcolor, string, FRAMEBUF_WIDTH);
}

void lcdconsole_puts_noblit(const char* string, int fgcolor, int bgcolor)
{
    while (*string) lcdconsole_putc_noblit(*string++, fgcolor, bgcolor);
}

void lcdconsole_update()
{
    uint32_t mode = enter_critical_section();
    if (displaylcd_busy())
    {
        lcdconsole_needs_update = true;
        leave_critical_section(mode);
        return;
    }
    leave_critical_section(mode);
    displaylcd((LCD_WIDTH - FRAMEBUF_WIDTH) / 2,
               (LCD_WIDTH - FRAMEBUF_WIDTH) / 2 + FRAMEBUF_WIDTH - 1,
               (LCD_HEIGHT - FRAMEBUF_HEIGHT) / 2,
               (LCD_HEIGHT - FRAMEBUF_HEIGHT) / 2 + FRAMEBUF_HEIGHT - 1,
               framebuf, 0);
}

void lcdconsole_puts(const char* string, int fgcolor, int bgcolor)
{
    while (*string) lcdconsole_putc_noblit(*string++, fgcolor, bgcolor);
    lcdconsole_update();
}

void lcdconsole_callback()
{
    if (lcdconsole_needs_update)
    {
        displaylcd((LCD_WIDTH - FRAMEBUF_WIDTH) / 2,
                   (LCD_WIDTH - FRAMEBUF_WIDTH) / 2 + FRAMEBUF_WIDTH - 1,
                   (LCD_HEIGHT - FRAMEBUF_HEIGHT) / 2,
                   (LCD_HEIGHT - FRAMEBUF_HEIGHT) / 2 + FRAMEBUF_HEIGHT - 1,
                   framebuf, 0);
        lcdconsole_needs_update = false;
    }
}
