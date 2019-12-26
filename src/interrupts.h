#pragma once

#include "irq.h"

__attribute__((naked)) void common_interrupt_stub()
{
  asm volatile(
      "pusha \n"          // Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
      "mov %ds, %ax \n"   // Set lower 16-bits of eax to ds (data segment selector)
      "push %eax \n"      // Save the ds
      "push %esp \n"      // Save the pointer to the register_info struct on the stack
      "mov $0x10, %ax \n" // Load the kernel data segment descriptor
      "mov %ax, %ds \n"
      "mov %ax, %es \n"
      "mov %ax, %fs \n"
      "mov %ax, %gs \n"
      "call interrupt_handler \n"
      "pop %eax \n" // Get rid of pushed pointer (0xdead)
      "pop %eax \n"
      "mov %ax, %es \n"
      "mov %ax, %ds \n"
      "mov %ax, %fs \n"
      "mov %ax, %gs \n"
      "popa \n"
      "add $8, %esp \n" // Cleans up the pushed error code and pushed IRQ number
      "iret"            // Pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
  );
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

ISR_NO_ERRORCODE(3)