#include "syscall.h"
#include "interrupt.h"
#include "serial_port.h"
#include "terminal.h"
#include "process.h"
#include <libk/assert.h>

//#define SYSCALL_DEBUG

volatile u32 *syscall_return = 0;

typedef struct iovec
{
  void *base;    // Starting address
  size_t length; // Number of bytes to transfer
} iovec;

void syscall_interrupt_handler(register_info *);
void sys$exit(i32);
int sys$write(int, char *, size_t);
int sys$writev(int, const iovec *, int);
int sys$ioctl(int, u32, void *);

void syscall_init()
{
  set_interrupt_callback(SYSCALL_INTERRUPT, syscall_interrupt_handler);
}

void syscall_interrupt_handler(register_info *info)
{
  u32 syscall_number = info->eax;

  switch (syscall_number)
  {
  case SYSCALL_EXIT:
    sys$exit(info->ebx);
    break;
  case SYSCALL_WRITE:
    *syscall_return = sys$write(info->ebx, (char *)info->ecx, info->edx);
    break;
  case SYSCALL_IOCTL:
    *syscall_return = sys$ioctl(info->ebx, info->ecx, (void *)info->edx);
    break;
  case SYSCALL_MODIFY_LDT:
    // TODO: sys$set_modify_ldt is not implemnted.
    break;
  case SYSCALL_WRITEV:
    *syscall_return = sys$writev(info->ebx, (const iovec *)info->ecx, info->edx);
    break;
  case SYSCALL_SET_THREAD_AREA:
    // TODO: sys$set_thread_area is not implemnted.
    break;
  case SYSCALL_EXIT_GROUP:
    // TODO: This should exit all threads but we have no concept of threads right now.
    sys$exit(info->ebx);
    break;
  case SYSCALL_SET_TID_ADDRESS:
    // TODO: sys$set_tid_address is not implemnted.
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

#define FD_STDOUT 1

int sys$write(int fd, char *buffer, size_t length)
{
  ASSERT(buffer);

  if (fd == FD_STDOUT)
  {
    for (size_t i = 0; i < length; i++)
    {
      terminal_putchar(buffer[i]);
    }
    return length;
  }

  serial_port_printf(COM1, "fd %d not implemented\n", fd);
  ASSERT_ALWAYS("");
}

#define TIOCGWINSZ 0x5413

int sys$ioctl(int fd, u32 request, void *arg)
{
  if (fd == 1 && request == TIOCGWINSZ)
  {
    return 0;
  }

  serial_port_printf(COM1, "Unimplemented ioctl request: %d\n", request);
  ASSERT_ALWAYS("");
}

int sys$writev(int fd, const iovec *iov, int iovcnt)
{
  ASSERT(iov);

  int total_bytes_written = 0;

  for (int i = 0; i < iovcnt; ++i)
  {
    int bytes_written = sys$write(fd, iov[i].base, iov[i].length);

    if (bytes_written < 0)
    {
      return bytes_written;
    }

    total_bytes_written += bytes_written;
  }

  return total_bytes_written;
}
