#include <string.h>
#include "shell.h"
#include "keyboard.h"
#include "screen.h"
#include "serial_port.h"

static shell_position cursor;
static char old_line_buffer[VGA_HEIGHT][VGA_WIDTH];
static char line_buffer[VGA_HEIGHT][VGA_WIDTH];

static void on_key_press(scan_code);
static char asciify_scan_code(u8);
static void shell_handle_overflow();
static void shell_redraw();
static void shell_update_cursor();

void shell_init()
{
  keyboard_add_key_press_callback(on_key_press);
}

static void on_key_press(scan_code code)
{
  char c = asciify_scan_code(code);

  if (code == LEFT_ARROW_PRESSED)
  {
    cursor.x--;
  }
  else if (code == RIGHT_ARROW_PRESSED)
  {
    cursor.x++;
  }

  serial_port_printf(COM1, "%s\n", line_buffer[1]);

  if (c > 0)
  {
    shell_putchar(c);
  }
  else
  {
    shell_handle_overflow();

    shell_update_cursor();
  }
}

void shell_putchar(char c)
{
  if (c == '\n')
  {
    cursor.y++;
    cursor.x = 0;
  }
  else
  {
    // TODO: memcpy is not necessary here, just tell shell_redraw which position to redraw.
    memcpy(&old_line_buffer, &line_buffer, sizeof(line_buffer));

    if (c == '\b')
    {
      line_buffer[(int)cursor.y][(int)--cursor.x] = ' ';
    }
    else
    {
      line_buffer[(int)cursor.y][(int)cursor.x++] = c;
    }

    shell_redraw();
  }

  shell_handle_overflow();

  shell_update_cursor();
}

static void shell_update_cursor()
{
  screen_set_cursor_position(cursor.x, cursor.y);
}

static void shell_redraw()
{
  for (size_t x = 0; x < VGA_WIDTH; x++)
  {
    for (size_t y = 0; y < VGA_WIDTH; y++)
    {
      char c = line_buffer[y][x];

      if (c != old_line_buffer[y][x])
      {
        screen_draw_char_at(x, y, c);
      }
    }
  }
}

static void shell_handle_overflow()
{
  if (cursor.x >= VGA_WIDTH)
  {
    cursor.y++;
    cursor.x = 0;
  }

  if (cursor.x < 0)
  {
    cursor.y--;
    cursor.x = VGA_WIDTH - 1;
  }

  if (cursor.y < 0)
  {
    cursor.y = 0;
    cursor.x = 0;
  }

  // TODO: y-overflow - scrolling
}

// Some code yoinked from https://github.com/levex/osdev/blob/master/drivers/keyboard.c

static char *qwertzuiop = "qwertzuiop";
static char *asdfghjkl = "asdfghjkl";
static char *yxcvbnm = "yxcvbnm";
static char *numbers = "123456789";

static char asciify_scan_code(u8 code)
{
  switch (code)
  {
  case ENTER_PRESSED:
    return '\n';
  case SPACE_PRESSED:
    return ' ';
  case BACKSPACE_PRESSED:
    return '\b';
  case POINT_PRESSED:
    return '.';
  case SLASH_RELEASED:
    return '/';
  case ZERO_PRESSED:
    return '0';
  }

  if (code >= ONE_PRESSED && code <= NINE_PRESSED)
  {
    return numbers[code - ONE_PRESSED];
  }

  if (code >= Q_PRESSED && code <= ENTER_PRESSED)
  {
    return qwertzuiop[code - 0x10];
  }
  else if (code >= A_PRESSED && code <= L_PRESSED)
  {
    return asdfghjkl[code - 0x1E];
  }
  else if (code >= Y_PRESSED && code <= M_PRESSED)
  {
    return yxcvbnm[code - 0x2C];
  }

  return 0;
}