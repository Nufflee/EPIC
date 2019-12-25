#include <string.h>
#include "io.h"

typedef enum
{
  COM1 = 0x3F8
} com_port;

void serial_port_init(com_port port);
size_t serial_port_printf(com_port port, char *string);
void serial_port_putchar(com_port port, char c);