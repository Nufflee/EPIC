#include <stddef.h>
#include "screen.h"
#include "io.h"
#include "serial_port.h"

static u16 *display_buffer = (u16 *)0xB8000;

void screen_init()
{
  screen_clear();

  // Enable cursor
  out8(0x3D4, 0x0A);
  out8(0x3D5, (in8(0x3D5) & 0xC0) | 0);
  out8(0x3D4, 0x0B);
  out8(0x3D5, (in8(0x3D5) & 0xE0) | 0);

  screen_set_cursor_position(0, 0);
}

void screen_clear()
{
  for (u16 x = 0; x < VGA_WIDTH; x++)
    for (u16 y = 0; y < VGA_HEIGHT; y++)
      display_buffer[screen_position_to_index(x,y)]  = '\0' | (0 << 8);
}

void screen_set_cursor_position(i8 x, i8 y)
{
  u16 index = screen_position_to_index(x,y);

  out8(0x3D4, 0x0F);
  out8(0x3D5, (u8)(index & 0xFF));
  out8(0x3D4, 0x0E);
  out8(0x3D5, (u8)((index >> 8) & 0xFF));
}

void screen_draw_char_at(u8 x, u8 y,u8 textcolour, u8 backcolour, char c)
{
  display_buffer[screen_position_to_index(x,y)] = c | (((backcolour << 4) | (textcolour & 0x0F)) << 8);
}

u16 screen_position_to_index(u8 x, u8 y)
{
  return (u16)(y * VGA_WIDTH + x);
}