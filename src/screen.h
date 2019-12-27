#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void screen_init();

void screen_clear();
void screen_draw_char_at(u8 x, u8 y, char c);

void screen_set_cursor_position(i8 x, i8 y);

u16 screen_position_to_index(u8 x, u8 y);