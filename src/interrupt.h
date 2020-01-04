#pragma once

#include <stddef.h>

typedef struct register_info
{
  u32 segment_selector;
  u32 edi, esi, ebp, useless_value, ebx, edx, ecx, eax; // Pushed by pusha.
  u32 int_number, err_code;                             // Interrupt number and error code (if applicable)
  u32 eip, cs, eflags, esp, ss;                         // Pushed by the processor automatically.
} register_info;

typedef enum interrupt_type
{
  KEYBOARD_INTERUPT = 33
} interrupt_type;

typedef void (*interrupt_callback)(register_info *);

void set_interrupt_callback(interrupt_type type, interrupt_callback callback);
void interrupt_handler(register_info *addr);