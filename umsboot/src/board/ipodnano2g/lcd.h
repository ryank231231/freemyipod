#ifndef __BOARD_IPODNANO2G_LCD_H__
#define __BOARD_IPODNANO2G_LCD_H__

#include "global.h"
#include "interface/lcd/lcd.h"
#include "interface/lcdif/lcdif.h"
#include "interface/backlight_manager/backlight_manager.h"


struct __attribute__((packed,aligned(4))) ipodnano2g_lcd_config
{
    const struct lcdif_instance* interface;
    const struct backlight_manager_instance* backlight;
};

struct __attribute__((packed,aligned(4))) ipodnano2g_lcd_state
{
};

extern const struct lcd_driver ipodnano2g_lcd_driver;


#endif
