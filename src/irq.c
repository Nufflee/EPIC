#include "irq.h"
#include "serial_port.h"
#include "printf.h"

// Interrupt exception codes: https://wiki.osdev.org/Exceptions
void interrupt_handler(register_info *info)
{
  char buffer[100];

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
    sprintf(buffer, "Interrupt '%d' handled with error code '%d'\n", info->int_number, info->err_code);
  }

  serial_port_printf(COM1, buffer);
}

void install_interrupt_handler(interrupt_type type, void (*callback)(register_info *))
{
  (void)type;
  (void)callback;
}