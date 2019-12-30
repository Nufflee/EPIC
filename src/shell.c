#include <string.h>
#include <stdbool.h>
#include "shell.h"
#include "keyboard.h"
#include "screen.h"
#include "serial_port.h"

static shell_position cursor_position;
static char old_line_buffer[VGA_HEIGHT * VGA_WIDTH];
static char line_buffer[VGA_HEIGHT * VGA_WIDTH];

static void on_key_press(scan_code);
static char asciify_scan_code(u8);
static void shell_handle_overflow();
static void shell_redraw();
static void shell_update_cursor();
static bool is_shell_position_greater(shell_position, shell_position);
static shell_range shell_get_current_line_range();
static void shell_shift_buffer_in_range(shell_range, int);
static u16 shell_position_to_index(shell_position);
static u16 shell_coordinate_to_index(u8, u8);
static shell_position shell_index_to_position(u16);
static void shell_handle_deletion_key(bool);

void shell_init()
{
  keyboard_add_key_press_callback(on_key_press);
}

static void on_key_press(scan_code code)
{
  char c = asciify_scan_code(code);

  switch (code)
  {
  case LEFT_ARROW_PRESSED:
    cursor_position.x--;
    break;
  case RIGHT_ARROW_PRESSED:
    cursor_position.x++;
    break;
  case BACKSPACE_PRESSED:
    shell_handle_deletion_key(false);
    break;
  case DELETE_PRESSED:
    shell_handle_deletion_key(true);
    break;
  default:
    break;
  }

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
    cursor_position.y++;
    cursor_position.x = 0;
  }
  else
  {
    memcpy(&old_line_buffer, &line_buffer, sizeof(line_buffer));

    char *char_at_cursor = &line_buffer[shell_position_to_index(cursor_position)];

    if (*char_at_cursor != '\0')
    {
      shell_range range = {cursor_position, shell_get_current_line_range().end};

      shell_shift_buffer_in_range(range, 1);
    }

    *char_at_cursor = c;

    cursor_position.x++;

    shell_redraw();
  }

  shell_handle_overflow();
  shell_update_cursor();
}

static void shell_handle_deletion_key(bool is_delete_key)
{
  shell_position line_end_position = shell_get_current_line_range().end;
  shell_position start_position = cursor_position;

  if (is_delete_key)
  {
    start_position.x++;
  }

  memcpy(&old_line_buffer, &line_buffer, sizeof(line_buffer));

  if (is_shell_position_greater(line_end_position, start_position))
  {
    shell_range range = {start_position, line_end_position};

    shell_shift_buffer_in_range(range, -1);
  }
  else
  {
    line_buffer[shell_coordinate_to_index(cursor_position.x - (is_delete_key ? 0 : 1), cursor_position.y)] = '\0';
  }

  if (!is_delete_key)
  {
    cursor_position.x--;
  }

  shell_redraw();
}

static shell_range shell_get_current_line_range()
{
  shell_position end_position = cursor_position;

  for (int i = shell_coordinate_to_index(0, cursor_position.y); i < VGA_WIDTH * VGA_HEIGHT; i++)
  {
    if (line_buffer[i] == '\0')
    {
      end_position = shell_index_to_position(i);

      break;
    }
  }

  shell_range result = {
      .start = {0, cursor_position.y},
      .end = end_position,
  };

  return result;
}

static bool is_shell_position_greater(shell_position position1, shell_position position2)
{
  if (position1.x > position2.x)
  {
    return position1.y >= position2.y;
  }

  return position1.y > position2.y;
}

static void shell_update_cursor()
{
  screen_set_cursor_position(cursor_position.x, cursor_position.y);
}

static void shell_redraw()
{
  for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
  {
    char c = line_buffer[i];

    if (c != old_line_buffer[i])
    {
      shell_position position = shell_index_to_position(i);

      screen_draw_char_at(position.x, position.y, c);
    }
  }
}

static void shell_handle_overflow()
{
  if (cursor_position.x >= VGA_WIDTH)
  {
    cursor_position.y++;
    cursor_position.x = 0;
  }

  if (cursor_position.x < 0)
  {
    cursor_position.y--;
    cursor_position.x = VGA_WIDTH - 1;
  }

  if (cursor_position.y < 0)
  {
    cursor_position.y = 0;
    cursor_position.x = 0;
  }

  // TODO: y-overflow - scrolling
}

static void shell_shift_buffer_in_range(shell_range range, int delta)
{
  u16 start_index = shell_position_to_index(range.start);
  u16 end_index = shell_position_to_index(range.end);

  if (delta > 0)
  {
    for (u16 i = end_index; i >= start_index; i--)
    {
      line_buffer[i + delta] = line_buffer[i];
    }
  }
  else
  {
    for (u16 i = start_index; i < end_index; i++)
    {
      line_buffer[i + delta] = line_buffer[i];
      line_buffer[i] = '\0';
    }
  }
}

static u16 shell_position_to_index(shell_position position)
{
  return shell_coordinate_to_index(position.x, position.y);
}

static u16 shell_coordinate_to_index(u8 x, u8 y)
{
  return y * VGA_WIDTH + x;
}

static shell_position shell_index_to_position(u16 index)
{
  shell_position result = {
      .x = index % VGA_WIDTH,
      .y = index / VGA_WIDTH,
  };

  return result;
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