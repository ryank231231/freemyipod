#include "global.h"
#include "lib/simpletextrenderer/simpletextrenderer.h"
#include "interface/textrenderer/textrenderer.h"
#include "interface/framebuffer/framebuffer.h"
#include "sys/util.h"


static const char simpletextrenderer_font_base = '!';
static const uint8_t simpletextrenderer_font[][5] =
{
    { 0x00, 0x00, 0x5f, 0x00, 0x00 },  // '!'
    { 0x00, 0x07, 0x00, 0x07, 0x00 },  // '"'
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },  // '#'
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },  // '$'
    { 0x23, 0x13, 0x08, 0x64, 0x62 },  // '%'
    { 0x36, 0x49, 0x55, 0x22, 0x50 },  // '&'
    { 0x05, 0x03, 0x00, 0x00, 0x00 },  // '''
    { 0x1c, 0x22, 0x41, 0x00, 0x00 },  // '('
    { 0x00, 0x00, 0x41, 0x22, 0x1c },  // ')'
    { 0x14, 0x08, 0x3e, 0x08, 0x14 },  // '*'
    { 0x08, 0x08, 0x3e, 0x08, 0x08 },  // '+'
    { 0x00, 0xa0, 0x60, 0x00, 0x00 },  // ','
    { 0x08, 0x08, 0x08, 0x08, 0x08 },  // '-'
    { 0x00, 0x60, 0x60, 0x00, 0x00 },  // '.'
    { 0x20, 0x10, 0x08, 0x04, 0x02 },  // '/'
    { 0x3e, 0x51, 0x49, 0x45, 0x3e },  // '0'
    { 0x00, 0x42, 0x7f, 0x40, 0x00 },  // '1'
    { 0x42, 0x61, 0x51, 0x49, 0x46 },  // '2'
    { 0x21, 0x41, 0x45, 0x4b, 0x31 },  // '3'
    { 0x18, 0x14, 0x12, 0x7f, 0x10 },  // '4'
    { 0x27, 0x45, 0x45, 0x45, 0x39 },  // '5'
    { 0x3c, 0x4a, 0x49, 0x49, 0x30 },  // '6'
    { 0x01, 0x71, 0x09, 0x05, 0x03 },  // '7'
    { 0x36, 0x49, 0x49, 0x49, 0x36 },  // '8'
    { 0x06, 0x49, 0x49, 0x29, 0x1e },  // '9'
    { 0x00, 0x36, 0x36, 0x00, 0x00 },  // ':'
    { 0x00, 0x56, 0x36, 0x00, 0x00 },  // ';'
    { 0x08, 0x14, 0x22, 0x41, 0x00 },  // '<'
    { 0x14, 0x14, 0x14, 0x14, 0x14 },  // '='
    { 0x00, 0x41, 0x22, 0x14, 0x08 },  // '>'
    { 0x02, 0x01, 0x51, 0x09, 0x06 },  // '?'
    { 0x32, 0x49, 0x79, 0x41, 0x3e },  // '@'
    { 0x7c, 0x12, 0x11, 0x12, 0x7c },  // 'A'
    { 0x7f, 0x49, 0x49, 0x49, 0x3e },  // 'B'
    { 0x3e, 0x41, 0x41, 0x41, 0x22 },  // 'C'
    { 0x7f, 0x41, 0x41, 0x22, 0x1c },  // 'D'
    { 0x7f, 0x49, 0x49, 0x49, 0x41 },  // 'E'
    { 0x7f, 0x09, 0x09, 0x09, 0x01 },  // 'F'
    { 0x3e, 0x41, 0x49, 0x49, 0x3a },  // 'G'
    { 0x7f, 0x08, 0x08, 0x08, 0x7f },  // 'H'
    { 0x00, 0x41, 0x7f, 0x41, 0x00 },  // 'I'
    { 0x20, 0x40, 0x41, 0x3f, 0x01 },  // 'J'
    { 0x7f, 0x08, 0x14, 0x22, 0x41 },  // 'K'
    { 0x7f, 0x40, 0x40, 0x40, 0x40 },  // 'L'
    { 0x7f, 0x02, 0x0c, 0x02, 0x7f },  // 'M'
    { 0x7f, 0x04, 0x08, 0x10, 0x7f },  // 'N'
    { 0x3e, 0x41, 0x41, 0x41, 0x3e },  // 'O'
    { 0x7f, 0x09, 0x09, 0x09, 0x06 },  // 'P'
    { 0x3e, 0x41, 0x51, 0x21, 0x5e },  // 'Q'
    { 0x7f, 0x09, 0x19, 0x29, 0x46 },  // 'R'
    { 0x26, 0x49, 0x49, 0x49, 0x32 },  // 'S'
    { 0x01, 0x01, 0x7f, 0x01, 0x01 },  // 'T'
    { 0x3f, 0x40, 0x40, 0x40, 0x3f },  // 'U'
    { 0x1f, 0x20, 0x40, 0x20, 0x1f },  // 'V'
    { 0x7f, 0x20, 0x18, 0x20, 0x7f },  // 'W'
    { 0x63, 0x14, 0x08, 0x14, 0x63 },  // 'X'
    { 0x03, 0x04, 0x78, 0x04, 0x03 },  // 'Y'
    { 0x61, 0x51, 0x49, 0x45, 0x43 },  // 'Z'
    { 0x00, 0x7f, 0x41, 0x41, 0x00 },  // '['
    { 0x02, 0x04, 0x08, 0x10, 0x20 },  // '\'
    { 0x00, 0x41, 0x41, 0x7f, 0x00 },  // ']'
    { 0x04, 0x02, 0x01, 0x02, 0x04 },  // '^'
    { 0x40, 0x40, 0x40, 0x40, 0x40 },  // '_'
    { 0x01, 0x02, 0x04, 0x00, 0x00 },  // '`'
    { 0x20, 0x54, 0x54, 0x54, 0x78 },  // 'a'
    { 0x7f, 0x44, 0x44, 0x44, 0x38 },  // 'b'
    { 0x38, 0x44, 0x44, 0x44, 0x28 },  // 'c'
    { 0x38, 0x44, 0x44, 0x44, 0x7f },  // 'd'
    { 0x38, 0x54, 0x54, 0x54, 0x18 },  // 'e'
    { 0x08, 0x7e, 0x09, 0x01, 0x02 },  // 'f'
    { 0x08, 0x54, 0x54, 0x54, 0x3c },  // 'g'
    { 0x7f, 0x04, 0x04, 0x04, 0x78 },  // 'h'
    { 0x00, 0x44, 0x7d, 0x40, 0x00 },  // 'i'
    { 0x20, 0x40, 0x40, 0x3d, 0x00 },  // 'j'
    { 0x7f, 0x10, 0x28, 0x44, 0x00 },  // 'k'
    { 0x00, 0x41, 0x7f, 0x40, 0x00 },  // 'l'
    { 0x7c, 0x04, 0x18, 0x04, 0x78 },  // 'm'
    { 0x7c, 0x08, 0x04, 0x04, 0x78 },  // 'n'
    { 0x38, 0x44, 0x44, 0x44, 0x38 },  // 'o'
    { 0x7c, 0x14, 0x14, 0x14, 0x18 },  // 'p'
    { 0x08, 0x14, 0x14, 0x14, 0x7c },  // 'q'
    { 0x7c, 0x08, 0x04, 0x04, 0x08 },  // 'r'
    { 0x48, 0x54, 0x54, 0x54, 0x20 },  // 's'
    { 0x04, 0x3f, 0x44, 0x40, 0x20 },  // 't'
    { 0x3c, 0x40, 0x40, 0x20, 0x7c },  // 'u'
    { 0x1c, 0x20, 0x40, 0x20, 0x1c },  // 'v'
    { 0x3c, 0x40, 0x38, 0x40, 0x3c },  // 'w'
    { 0x44, 0x28, 0x10, 0x28, 0x44 },  // 'x'
    { 0x0c, 0x50, 0x50, 0x50, 0x3c },  // 'y'
    { 0x44, 0x64, 0x54, 0x4c, 0x44 },  // 'z'
    { 0x00, 0x08, 0x36, 0x41, 0x00 },  // '{'
    { 0x00, 0x00, 0x77, 0x00, 0x00 },  // '|'
    { 0x00, 0x41, 0x36, 0x08, 0x00 },  // '}'
    { 0x02, 0x01, 0x02, 0x04, 0x02 },  // '~'
};

static void simpletextrenderer_init(const struct textrenderer_instance* instance)
{
}

static int simpletextrenderer_get_line_height(const struct textrenderer_instance* instance)
{
    return 8;
}

static int simpletextrenderer_get_max_width(const struct textrenderer_instance* instance)
{
    return 6;
}

static int simpletextrenderer_render_char(const struct textrenderer_instance* instance,
                                          const struct framebuffer_instance* fb, int x, int y, uint32_t color, char c)
{
    uint32_t idx = c - simpletextrenderer_font_base;
    if (idx >= ARRAYLEN(simpletextrenderer_font)) return 6;
    const uint8_t* font = simpletextrenderer_font[idx];
    int width = 5;
    while (width--)
    {
        int pixels = *font++;
        int i;
        for (i = 0; i < 8; i++)
        {
            if (pixels & 1) framebuffer_set_pixel(fb, x, y + i, color);
            pixels >>= 1;
        }
        x++;
    }
    return 6;
}


const struct textrenderer_driver simpletextrenderer_driver =
{
    .init = simpletextrenderer_init,
    .get_line_height = simpletextrenderer_get_line_height,
    .get_max_width = simpletextrenderer_get_max_width,
    .render_char = simpletextrenderer_render_char,
};
