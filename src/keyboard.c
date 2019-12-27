#include <stdio.h>
#include "keyboard.h"
#include "io.h"
#include "serial_port.h"
#include "interrupt.h"

#define CALLBACK_COUNT 12

static u16 last_scan_code;
static keyboard_callback key_press_callbacks[CALLBACK_COUNT];

static void keyboard_interrupt_handler();

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