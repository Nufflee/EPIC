#include "io.h"

void out8(u16 port, u8 value)
{
  asm volatile("outb %0, %1"
               :
               : "a"(value), "Nd"(port));
}

u8 in8(u16 port)
{
  u8 value;

  asm volatile("inb %1, %0"
               : "=a"(value)
               : "Nd"(port));

  return value;
}

void memset(u32 *dest, int val, u32 size)
{
    i32 i;
    u32 *poi = dest;
    for(i=0; i<size; i++)
    {
        *poi = val;
        poi++;
    }
}