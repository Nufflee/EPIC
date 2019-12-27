#include <stddef.h>
#include "terminal.h"
#include "io.h"
#include "serial_port.h"

static u16 *display_buffer = (u16 *)0xB8000;
static u16 x, y;

void terminal_clear()
{
  for (u16 x = 0; x < VGA_WIDTH; x++)
  {
    for (u16 y = 0; y < VGA_HEIGHT; y++)
    {
      display_buffer[y * VGA_WIDTH + x] = ' ' | (7 << 8);
    }
  }
}

void terminal_init()
{
  terminal_clear();

  // Enable cursor
  out8(0x3D4, 0x0A);
  out8(0x3D5, (in8(0x3D5) & 0xC0) | 0);
  out8(0x3D4, 0x0B);
  out8(0x3D5, (in8(0x3D5) & 0xE0) | 0);

  terminal_set_cursor_position(x, y);
}

void terminal_set_cursor_position(int x, int y)
{
  u16 position = (y + 1) * VGA_WIDTH + x;

  out8(0x3D4, 0x0F);
  out8(0x3D5, (u8)(position & 0xFF));
  out8(0x3D4, 0x0E);
  out8(0x3D5, (u8)((position >> 8) & 0xFF));
}

void terminal_putchar(char c)
{
  if (c == '\n')
  {
    y++;
    x = 0;
  }
  else
  {
    // TODO: Proper color handling
    display_buffer[y * VGA_WIDTH + x] = c | (7 << 8);
    x++;
  }

  if (x >= VGA_WIDTH)
  {
    y++;
    x = 0;
  }

  terminal_set_cursor_position(x, y);

  // TODO: y-overflow - scrolling
}