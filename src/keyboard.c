#include <stdio.h>
#include "keyboard.h"
#include "io.h"
#include "serial_port.h"
#include "terminal.h"
#include "interrupt.h"

// Some code yoinked from https://github.com/levex/osdev/blob/master/drivers/keyboard.c

static char *qwertzuiop = "qwertzuiop";
static char *asdfghjkl = "asdfghjkl";
static char *yxcvbnm = "yxcvbnm";
static char *numbers = "123456789";

#define CALLBACK_COUNT 12

static u16 last_scan_code;
static keyboard_callback key_press_callbacks[CALLBACK_COUNT];

static void keyboard_interrupt_handler();
static char asciify_scan_code(u8 code);

void keyboard_init()
{
  set_interrupt_callback(KEYBOARD_INTERUPT, keyboard_interrupt_handler);
}

static void keyboard_interrupt_handler()
{
  u16 scan_code = in8(0x60);

  if (scan_code != 0xe0)
  {
    if (last_scan_code == 0xE0)
    {
      scan_code = 0xE000 | scan_code;
    }

    for (int i = 0; i < 12; i++)
    {
      keyboard_callback callback = key_press_callbacks[i];

      if (callback)
      {
        callback(scan_code);
      }
    }

    char c = asciify_scan_code(scan_code);

    serial_port_printf(COM1, "scan code: %d (0x%x)\n", scan_code, scan_code);

    if (c > 0)
    {
      serial_port_printf(COM1, "%d\n", c);

      terminal_putchar(c);
    }
  }

  last_scan_code = scan_code;
}

void keyboard_add_key_press_callback(keyboard_callback callback)
{
  static u8 last_callback_index = 0;

  if (last_callback_index >= 11)
  {
    serial_port_printf(COM1, "No more key press callback slots!");

    return;
  }

  key_press_callbacks[last_callback_index++] = callback;
}

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
    return numbers[code - ONE_PRESSED];

  if (code >= Q_PRESSED && code <= ENTER_PRESSED)
    return qwertzuiop[code - 0x10];
  else if (code >= A_PRESSED && code <= L_PRESSED)
    return asdfghjkl[code - 0x1E];
  else if (code >= Y_PRESSED && code <= M_PRESSED)
    return yxcvbnm[code - 0x2C];

  return 0;
}