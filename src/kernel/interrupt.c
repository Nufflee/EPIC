#include <stdio.h>
#include "interrupt.h"
#include "serial_port.h"
#include "pic.h"

static interrupt_callback callback_map[256];

void set_interrupt_callback(interrupt_type type, interrupt_callback callback)
{
  callback_map[type] = callback;
}

// Interrupt exception codes: https://wiki.osdev.org/Exceptions
void interrupt_handler(register_info *info)
{
  interrupt_callback callback = callback_map[info->int_number];

  if (callback != 0)
  {
    callback(info);
  }
  else
  {
    if (info->int_number == 13)
    {
      u8 table = (((1 << 2) - 1) & (info->err_code >> (2 - 1)));

      if (table == 0b01 || table == 0b11)
      {
        serial_port_printf(COM1, "Interrupt handler for interrupt '%d' not found!\n", (((1 << 12) - 1) & (info->err_code >> (4 - 1))));
      }
    }
    else
    {
      serial_port_printf(COM1, "Interrupt '%d' handled with error code '%d' but no callback was set.\n", info->int_number, info->err_code);
    }
  }

  if (info->int_number >= 32 && info->int_number < 48)
  {
    pic_send_eoi(info->int_number - 32);
  }
}