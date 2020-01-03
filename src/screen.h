#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
enum VGA_COLOURS
{
    WHITE = (u8)15,
    YELLOW = (u8)14,
    LIGHT_MAGENTA = (u8)13,
    LIGHT_RED = (u8)12,
    LIGHT_CYAN = (u8)11,
    LIGHT_GREEN = (u8)10,
    LIGHT_BLUE = (u8)9,
    DARK_GRAY = (u8)8,
    LIGHT_GRAY = (u8)7,
    BROWN = (u8)6,
    MAGENTA = (u8)5,
    RED = (u8)4,
    CYAN = (u8)3,
    GREEN = (u8)2,
    BLUE = (u8)1,
    BLACK = (u8)0
};
void screen_init();
void screen_clear();
void screen_draw_char_at(u8 x, u8 y,u8 textcolour, u8 backcolour, char c);
void screen_set_cursor_position(i8 x, i8 y);
u16 screen_position_to_index(u8 x, u8 y);