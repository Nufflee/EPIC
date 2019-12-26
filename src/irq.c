#include "irq.h"
#include "serial_port.h"
#include "printf.h"

void interrupt_handler(register_info *info)
{
  char buffer[100];
  sprintf(buffer, "irq %d handled with error code %d\n", info->int_number, info->err_code);
  serial_port_printf(COM1, buffer);
}

void install_interrupt_handler(interrupt_type type, void (*callback)(register_info *))
{
  (void)type;
  (void)callback;
}