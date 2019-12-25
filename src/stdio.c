#include <string.h>
#include "terminal.h"

size_t printf(char *message)
{
  size_t len = strlen(message);

  for (size_t i = 0; i < len; i++)
  {
    terminal_putchar(message[i]);
  }

  return len;
}