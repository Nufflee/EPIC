#include "keyboard.h"
#include "io.h"
#include "serial_port.h"
#include "terminal.h"
#include "printf.h"

// Some code yoinked from https://github.com/levex/osdev/blob/master/drivers/keyboard.c

static char *qwertzuiop = "qwertzuiop";
static char *asdfghjkl = "asdfghjkl";
static char *yxcvbnm = "yxcvbnm";
static char *numbers = "123456789";

void keyboard_init()
{
}

static u8 scancode_to_ascii(u8 code)
{
  switch (code)
  {
  case 0x1C:
    return '\n';
  case 0x39:
    return ' ';
  case 0x0E:
    return '\r';
  case POINT_PRESSED:
    return '.';
  case SLASH_RELEASED:
    return '/';
  case ZERO_PRESSED:
    return '0';
  }

  if (code >= ONE_PRESSED && code <= NINE_PRESSED)
    return numbers[code - ONE_PRESSED];

  if (code >= 0x10 && code <= 0x1C)
    return qwertzuiop[code - 0x10];
  else if (code >= 0x1E && code <= 0x26)
    return asdfghjkl[code - 0x1E];
  else if (code >= 0x2C && code <= 0x32)
    return yxcvbnm[code - 0x2C];

  return 0;
}

void keyboard_start_polling()
{
  char code;

  while (1)
  {
    if (code == in8(0x60))
    {
      continue;
    }

    code = in8(0x60);

    if (code & 0x80 || code == 0x8)
    {
      continue;
    }

    char buffer[100];

    sprintf(buffer, "key pressed: 0x%x, %d\n", code, code);

    serial_port_printf(COM1, buffer);

    sprintf(buffer, "0x64 port: 0x%x\n", in8(0x64));

    serial_port_printf(COM1, buffer);

    char c = scancode_to_ascii(code);

    if (c > 0)
    {
      terminal_putchar(c);
    }
  }
}