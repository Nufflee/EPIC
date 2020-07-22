#pragma once

#include "interrupt.h"

extern volatile u32 *syscall_return;

__attribute__((naked)) void common_interrupt_stub()
{
  asm volatile(
      "pusha;" // Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
      "mov %%esp, %%eax;"
      "add $0x1c, %%eax;" // esp points to edi here, eax is the 7th register on the stack
      "mov %%eax, (%0);"  // Save the address of where the original userspace eax is on the stack.
                          // This allows us to overwrite it when needed, for example for syscalls that have return values.
      "mov %%ds, %%ax;"   // Set lower 16-bits of eax to ds (data segment selector)
      "push %%eax;"       // Save the ds
      "push %%esp;"       // Save the pointer to the register_info struct on the stack
      "mov $0x10, %%ax;"  // Load the kernel data segment descriptor so we can access kernel memory
      "mov %%ax, %%ds;"
      "mov %%ax, %%es;"
      "mov %%ax, %%fs;"
      "mov %%ax, %%gs;"
      "call interrupt_handler;"
      "pop %%eax;" // Get rid of pushed pointer
      "pop %%eax;"
      "mov %%ax, %%es;"
      "mov %%ax, %%ds;"
      "mov %%ax, %%fs;"
      "mov %%ax, %%gs;"
      "popa;"
      "add $8, %%esp;" // Cleans up the pushed error code and pushed IRQ number
      "iret;"          // Pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
      : "=m"(syscall_return));
}

#define ISR_NO_ERRORCODE(N)                \
  __attribute__((naked)) void isr##N(void) \
  {                                        \
    asm volatile(                          \
        "cli \n"                           \
        "push $0 \n"                       \
        "push $" #N "\n"                   \
        "jmp common_interrupt_stub \n");   \
  }

#define ISR_WITH_ERRORCODE(N)              \
  __attribute__((naked)) void isr##N(void) \
  {                                        \
    asm volatile(                          \
        "cli \n"                           \
        "push $" #N "\n"                   \
        "jmp common_interrupt_stub \n");   \
  }

ISR_NO_ERRORCODE(0)
ISR_NO_ERRORCODE(1)
ISR_NO_ERRORCODE(2)
ISR_NO_ERRORCODE(3)
ISR_NO_ERRORCODE(4)
ISR_NO_ERRORCODE(5)
ISR_NO_ERRORCODE(6)
ISR_NO_ERRORCODE(7)
ISR_WITH_ERRORCODE(8)
ISR_NO_ERRORCODE(9)
ISR_WITH_ERRORCODE(10)
ISR_WITH_ERRORCODE(11)
ISR_WITH_ERRORCODE(12)
ISR_WITH_ERRORCODE(13)
ISR_WITH_ERRORCODE(14)
ISR_NO_ERRORCODE(15)
ISR_NO_ERRORCODE(16)
ISR_WITH_ERRORCODE(17)
ISR_NO_ERRORCODE(18)
ISR_NO_ERRORCODE(19)
ISR_NO_ERRORCODE(20)
ISR_NO_ERRORCODE(21)
ISR_NO_ERRORCODE(22)
ISR_NO_ERRORCODE(23)
ISR_NO_ERRORCODE(24)
ISR_NO_ERRORCODE(25)
ISR_NO_ERRORCODE(26)
ISR_NO_ERRORCODE(27)
ISR_NO_ERRORCODE(28)
ISR_NO_ERRORCODE(29)
ISR_WITH_ERRORCODE(30)
ISR_NO_ERRORCODE(31)

ISR_NO_ERRORCODE(33)

ISR_NO_ERRORCODE(128)