#include "syscall.h"
#include "interrupt.h"
#include "serial_port.h"
#include "terminal.h"
#include "process.h"

//#define SYSCALL_DEBUG

void syscall_interrupt_handler(register_info *);
void sys$write(u32, char *, size_t);

void syscall_init()
{
  set_interrupt_callback(SYSCALL_INTERRUPT, syscall_interrupt_handler);
}

void syscall_interrupt_handler(register_info *info)
{
  u32 syscall_number = info->eax;

  if (syscall_number == SYSCALL_WRITE)
  {
    sys$write(info->ebx, (char *)info->ecx, info->edx);
  }

#ifdef SYSCALL_DEBUG
  serial_port_printf(COM1, "Syscall - number: %d, ebx: %d, ecx: %d, edx: %d\n", info->eax, info->ebx, info->ecx, info->edx);
#endif
}

void sys$write(u32 fd, char *buffer, size_t length)
{
  buffer += PROCESS_BASE_ADDRESS;

  if (fd == 1)
  {
    for (size_t i = 0; i < length; i++)
    {
      terminal_putchar(buffer[i]);
    }
  }
}