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
  PLACEHOLDER_INTERRUPT = 0x0
} interrupt_type;

void interrupt_handler(register_info *addr);
void install_interrupt_handler(interrupt_type type, void (*callback)(register_info *));