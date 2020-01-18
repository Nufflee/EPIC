#include "math.h"

u32 divide_and_round_up(const u32 x, const u32 y)
{
  return (x + y - 1) / y;
}