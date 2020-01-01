#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "terminal.h"
#include "keyboard.h"
#include "screen.h"
#include "serial_port.h"

#define PROMPT "> "

static terminal_position cursor_position;
static char old_line_buffer[VGA_HEIGHT * VGA_WIDTH];
static char line_buffer[VGA_HEIGHT * VGA_WIDTH];

static void on_key_press(scan_code);
static void terminal_handle_overflow();
static void terminal_redraw();
static void terminal_update_cursor();
static bool is_terminal_position_greater(terminal_position, terminal_position);
static terminal_range terminal_get_current_line_range();
static void terminal_shift_buffer_in_range(terminal_range, int);
static u16 terminal_position_to_index(terminal_position);
static u16 terminal_coordinate_to_index(u8, u8);
static terminal_position terminal_index_to_position(u16);
static void terminal_handle_deletion_key(bool);
static char terminal_get_current_char();
static terminal_range terminal_get_editable_range();

void terminal_init()
{
  keyboard_add_key_press_callback(on_key_press);

  printf(PROMPT);
}

static void on_key_press(scan_code code)
{
  switch (code)
  {
  case LEFT_ARROW_PRESSED:
    cursor_position.x--;
    break;
  case RIGHT_ARROW_PRESSED:
    if (terminal_get_current_char() != '\0')
    {
      cursor_position.x++;
    }

    break;
  case BACKSPACE_PRESSED:
    terminal_handle_deletion_key(false);
    break;
  case DELETE_PRESSED:
    terminal_handle_deletion_key(true);
    break;
  case ENTER_PRESSED:
    cursor_position.x = 0;
    cursor_position.y++;

    printf(PROMPT);

    break;
  default:
  {
    char c = asciify_scan_code(code);

    if (c > 0)
    {
      terminal_putchar(c);
    }

    return;
  }
  }

  terminal_range editable_range = terminal_get_editable_range();

  if (cursor_position.x < editable_range.start.x && cursor_position.y == editable_range.start.y)
  {
    cursor_position.x++;
  }
  else if (cursor_position.x > editable_range.end.x && cursor_position.y == editable_range.end.y)
  {
    cursor_position.x--;
  }

  terminal_handle_overflow();
  terminal_update_cursor();
}

void terminal_putchar(char c)
{
  if (c == '\n')
  {
    cursor_position.x = 0;
    cursor_position.y++;
  }
  else
  {
    char *char_at_cursor = &line_buffer[terminal_position_to_index(cursor_position)];

    memcpy(&old_line_buffer, &line_buffer, sizeof(line_buffer));

    if (*char_at_cursor != '\0')
    {
      terminal_range range = {cursor_position, terminal_get_current_line_range().end};

      terminal_shift_buffer_in_range(range, 1);
    }

    *char_at_cursor = c;

    cursor_position.x++;

    terminal_redraw();
  }

  terminal_handle_overflow();
  terminal_update_cursor();
}

static void terminal_handle_deletion_key(bool is_delete_key)
{
  terminal_position line_end_position = terminal_get_current_line_range().end;
  terminal_position start_position = cursor_position;

  if (is_delete_key)
  {
    start_position.x++;
  }

  memcpy(&old_line_buffer, &line_buffer, sizeof(line_buffer));

  if (is_terminal_position_greater(line_end_position, start_position))
  {
    terminal_range range = {start_position, line_end_position};

    terminal_shift_buffer_in_range(range, -1);
  }
  else
  {
    line_buffer[terminal_coordinate_to_index(cursor_position.x - (is_delete_key ? 0 : 1), cursor_position.y)] = '\0';
  }

  if (!is_delete_key)
  {
    cursor_position.x--;
  }

  terminal_redraw();
}

static terminal_range terminal_get_editable_range()
{
  terminal_position prompt_position;

  for (int i = VGA_HEIGHT * VGA_WIDTH; i >= 0; i--)
  {
    if (line_buffer[i] == '>')
    {
      prompt_position = terminal_index_to_position(i + 2);

      break;
    }
  }

  terminal_range range = {
      .start = prompt_position,
      .end = {VGA_WIDTH - 1, VGA_HEIGHT - 1},
  };

  return range;
}

static terminal_range terminal_get_current_line_range()
{
  terminal_position end_position = cursor_position;

  for (int i = terminal_coordinate_to_index(0, cursor_position.y); i < VGA_WIDTH * VGA_HEIGHT; i++)
  {
    if (line_buffer[i] == '\0')
    {
      end_position = terminal_index_to_position(i);

      break;
    }
  }

  terminal_range result = {
      .start = {0, cursor_position.y},
      .end = end_position,
  };

  return result;
}

static bool is_terminal_position_greater(terminal_position position1, terminal_position position2)
{
  if (position1.x > position2.x)
  {
    return position1.y >= position2.y;
  }

  return position1.y > position2.y;
}

static void terminal_update_cursor()
{
  screen_set_cursor_position(cursor_position.x, cursor_position.y);
}

static void terminal_redraw()
{
  for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
  {
    char c = line_buffer[i];

    if (c != old_line_buffer[i])
    {
      terminal_position position = terminal_index_to_position(i);

      screen_draw_char_at(position.x, position.y, c);
    }
  }
}

static void terminal_handle_overflow()
{
  if (cursor_position.x >= VGA_WIDTH)
  {
    cursor_position.x = 0;
    cursor_position.y++;
  }

  if (cursor_position.x < 0)
  {
    cursor_position.y--;
    cursor_position.x = VGA_WIDTH - 1;
  }

  if (cursor_position.y < 0)
  {
    cursor_position.x = 0;
    cursor_position.y = 0;
  }

  // TODO: y-overflow - scrolling
}

static char terminal_get_current_char()
{
  return line_buffer[terminal_position_to_index(cursor_position)];
}

static void terminal_shift_buffer_in_range(terminal_range range, int delta)
{
  // TODO: Why not implement this with memcpy instead?

  u16 start_index = terminal_position_to_index(range.start);
  u16 end_index = terminal_position_to_index(range.end);

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

static u16 terminal_position_to_index(terminal_position position)
{
  return terminal_coordinate_to_index(position.x, position.y);
}

static u16 terminal_coordinate_to_index(u8 x, u8 y)
{
  return y * VGA_WIDTH + x;
}

static terminal_position terminal_index_to_position(u16 index)
{
  terminal_position result = {
      .x = index % VGA_WIDTH,
      .y = index / VGA_WIDTH,
  };

  return result;
}