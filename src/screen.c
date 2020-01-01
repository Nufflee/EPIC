#include <stddef.h>
#include "screen.h"
#include "io.h"
#include "serial_port.h"

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
  u16 attrib = (0 << 4) | (0 & 0x0F);
  volatile u16 * where;

  for (u16 x = 0; x < VGA_WIDTH; x++)
  {
    for (u16 y = 0; y < VGA_HEIGHT; y++)
    {
      
     where = (volatile u16 *)0xB8000 + (y * VGA_WIDTH + x) ;
     *where = '\0' | (attrib << 8);
    }
  }
}

void screen_set_cursor_position(i8 x, i8 y)
{
  u16 index = (u16)(y * VGA_WIDTH + x);

  out8(0x3D4, 0x0F);
  out8(0x3D5, (u8)(index & 0xFF));
  out8(0x3D4, 0x0E);
  out8(0x3D5, (u8)((index >> 8) & 0xFF));
}

void screen_draw_char_at(u8 x, u8 y,u8 forecolour, u8 backcolour, char c)
{
  u16 attrib = (backcolour << 4) | (forecolour & 0x0F);
     volatile u16 * where;
     where = (volatile u16 *)0xB8000 + (y * VGA_WIDTH + x) ;
     *where = c | (attrib << 8);
}