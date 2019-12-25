#include <stddef.h>

size_t strlen(char *string)
{
  char c = string[0];
  size_t len = 0;

  while (c != '\0')
  {
    len++;
    c = string[len];
  }

  return len;
}