#pragma once

#include <stddef.h>

typedef struct terminal_position
{
  i8 x, y;
} terminal_position;

typedef struct terminal_range
{
  terminal_position start;
  terminal_position end;
} terminal_range;

void terminal_init();
void terminal_putchar(char c);