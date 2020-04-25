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

u16 in16(u16 port)
{
  u16 value;

  asm volatile("inw %1, %0"
               : "=a"(value)
               : "Nd"(port));

  return value;
}