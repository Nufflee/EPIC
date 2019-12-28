#include <stddef.h>

typedef struct shell_position
{
  i8 x, y;
} shell_position;

typedef struct shell_range
{
  shell_position start;
  shell_position end;
} shell_range;

void shell_init();
void shell_putchar(char c);