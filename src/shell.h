#include <stddef.h>

typedef struct shell_position
{
  i8 x, y;
} shell_position;

void shell_init();
void shell_putchar(char c);