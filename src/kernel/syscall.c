#include "syscall.h"
#include "interrupt.h"
#include "serial_port.h"
#include "terminal.h"
#include "process.h"
#include <libk/assert.h>

//#define SYSCALL_DEBUG

void syscall_interrupt_handler(register_info *);
void sys$exit(i32);
void sys$write(u32, char *, size_t);

void syscall_init()
{
  set_interrupt_callback(SYSCALL_INTERRUPT, syscall_interrupt_handler);
}

void syscall_interrupt_handler(register_info *info)
{
  u32 syscall_number = info->eax;

  switch (syscall_number)
  {
  case SYSCALL_WRITE:
    sys$write(info->ebx, (char *)info->ecx, info->edx);
    break;
  case SYSCALL_EXIT:
    sys$exit(info->ebx);
    break;
  case SYSCALL_MODIFY_LDT:
    // TODO: sys$set_modify_ldt is not implemnted.
    // TODO: This syscall is supposed to return something.
    break;
  case SYSCALL_SET_THREAD_AREA:
    // TODO: sys$set_thread_area is not implemnted.
    // TODO: This syscall is supposed to return something.
    break;
  case SYSCALL_EXIT_GROUP:
    // TODO: This should exit all threads but we have no concept of threads right now.
    sys$exit(info->ebx);
    break;
  case SYSCALL_SET_TID_ADDRESS:
    // TODO: sys$set_tid_address is not implemnted.
    // TODO: This syscall is supposed to return something.
    break;
  default:
    // TODO: ASSERT_ALWAYS formatting
    serial_port_printf(COM1, "Syscall number '%d' doesn't exist\n", syscall_number);
    ASSERT_ALWAYS("");
    break;
  }

#ifdef SYSCALL_DEBUG
  serial_port_printf(COM1, "Syscall - number: %d, ebx: %d, ecx: %d, edx: %d\n", info->eax, info->ebx, info->ecx, info->edx);
#endif
}

void sys$exit(i32 return_code)
{
  // Save return code into 'eax' so it can be read by the kernel
  asm volatile("movl %0, %%eax" ::"m"(return_code));

  // Jump back to where we left off in the kernel
  asm volatile("jmp process_kernel_return");
}

void sys$write(u32 fd, char *buffer, size_t length)
{
  if (fd == 1)
  {
    for (size_t i = 0; i < length; i++)
    {
      terminal_putchar(buffer[i]);
    }
  }
}