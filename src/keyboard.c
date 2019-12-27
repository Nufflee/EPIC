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

static void keyboard_interrupt_handler();
static u8 scancode_to_ascii(u8 code);

void keyboard_init()
{
  set_interrupt_callback(KEYBOARD_INTERUPT, keyboard_interrupt_handler);
}

static void keyboard_interrupt_handler()
{
  u8 scan_code = in8(0x60);
  char c = scancode_to_ascii(scan_code);

  if (c > 0)
  {
    serial_port_printf(COM1, "%d\n", c);

    terminal_putchar(c);
  }
}

static u8 scancode_to_ascii(u8 code)
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