#include <stddef.h>
#include "terminal.h"

static u16 *display_buffer = (u16 *)0xB8000;
static u16 width = 80;
static u16 height = 25;
static u16 x, y;

void terminal_clear()
{
  for (u16 x = 0; x < width; x++)
  {
    for (u16 y = 0; y < height; y++)
    {
      display_buffer[y * width + x] = 0;
    }
  }
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
    display_buffer[y * width + x] = c | (4 << 8);
    x++;
  }

  if (x >= width)
  {
    y++;
    x = 0;
  }

  // TODO: y-overflow - scrolling
}