#include <stdio.h>
#include <string.h>
#include <kernel/terminal.h>
#include <libk/assert.h>
#include "printf.h"

int printf(const char *format, ...)
{
  char buffer[BUFFER_LEN];

  va_list list;
  va_start(list, format);

  int len = vsnprintf(buffer, sizeof(buffer), format, list);

  ASSERT(len < BUFFER_LEN);

  for (int i = 0; i < len; i++)
  {
    terminal_putchar(buffer[i]);
  }

  va_end(list);

  return len;
}

int sprintf(char *buffer, const char *format, ...)
{
  va_list list;
  va_start(list, format);

  int len = vsnprintf(buffer, BUFFER_LEN, format, list);

  va_end(list);

  return len;
}

int vsprintf(char *buffer, const char *format, va_list list)
{
  return vsnprintf(buffer, BUFFER_LEN, format, list);
}

int snprintf(char *buffer, size_t length, const char *format, ...)
{
  va_list list;
  va_start(list, format);

  int len = vsnprintf(buffer, length, format, list);

  va_end(list);

  return len;
}

int vsnprintf(char *buffer, size_t length, const char *format, va_list list)
{
  return vsnprintf_(buffer, length, format, list);
}