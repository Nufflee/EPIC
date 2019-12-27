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

void *memset(void *buffer, int value, size_t length)
{
  u8 *dest = buffer;

  while (length--)
    *dest++ = (u8)value;

  return dest;
}

void *memcpy(void *destination, void *source, size_t length)
{
  char *csrc = (char *)source;
  char *cdest = (char *)destination;

  for (size_t i = 0; i < length; i++)
  {
    cdest[i] = csrc[i];
  }

  return destination;
}