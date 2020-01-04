#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "serial_port.h"
#include "io.h"

void serial_port_init(com_port port)
{
  out8(port + 1, 0x00); // Disable all interrupts
  out8(port + 3, 0x80); // Enable DLAB (baud rate divisor)
  out8(port + 0, 0x03); // Set low byte of baud rate divisor to 3 (38400 baud)
  out8(port + 1, 0x00); // Set high byte of baud rate divisor to 0
  out8(port + 3, 0x03); // 8 bits, no parity, one stop bit
  out8(port + 2, 0xC7); // Enable FIFO, clear it, 14-byte threshold
  out8(port + 4, 0x0B); // Enabled interrupt, set RTS/DSR
}

int serial_port_printf(com_port port, const char *format, ...)
{
  va_list list;
  va_start(list, format);

  char buffer[BUFFER_LEN];

  int length = vsnprintf(buffer, sizeof(buffer), format, list);

  for (int i = 0; i < length; i++)
  {
    serial_port_putchar(port, buffer[i]);
  }

  va_end(list);

  return length;
}

static int serial_port_is_tx_buffer_empty(com_port port)
{
  return in8(port + 5) & 0x20;
}

void serial_port_putchar(com_port port, char c)
{
  // Wait until we can send
  while (!serial_port_is_tx_buffer_empty(port))
  {
  }

  out8(port, c);
}