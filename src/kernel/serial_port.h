#pragma once

#include <string.h>
#include "io.h"

typedef enum
{
  COM1 = 0x3F8
} com_port;

void serial_port_init(com_port port);
int serial_port_printf(com_port port, const char *format, ...);
void serial_port_putchar(com_port port, char c);