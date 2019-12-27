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
    char buffer[BUFFER_LEN];

    if (info->int_number == 13)
    {
      u8 table = (((1 << 2) - 1) & (info->err_code >> (2 - 1)));

      if (table == 0b01 || table == 0b11)
      {
        sprintf(buffer, "Interrupt handler for interrupt '%d' not found!\n", (((1 << 12) - 1) & (info->err_code >> (4 - 1))));
      }
    }
    else
    {
      sprintf(buffer, "Interrupt '%d' handled with error code '%d' but no callback was set.\n", info->int_number, info->err_code);
    }

    serial_port_printf(COM1, buffer);
  }

  if (info->int_number >= 32 && info->int_number < 48)
  {
    pic_send_eoi(info->int_number - 32);
  }
}