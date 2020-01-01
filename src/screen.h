#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void screen_init();
void screen_clear();
void screen_draw_char_at(u8 x, u8 y,u8 forecolour, u8 backcolour, char c);
void screen_set_cursor_position(i8 x, i8 y);