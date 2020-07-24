#include "syscall.h"
#include "interrupt.h"
#include "serial_port.h"
#include "terminal.h"
#include "process.h"
#include "memory_manager.h"
#include "process.h"
#include <libk/math.h>
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
int sys$brk(void *);
int sys$ioctl(int, u32, void *);
int sys$munmap(void *, size_t);
int sys$writev(int, const iovec *, int);
void *sys$mmap(void *, size_t, int, int, int, u32);

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
  case SYSCALL_BRK:
    *syscall_return = sys$brk((void *)info->ebx);
    break;
  case SYSCALL_IOCTL:
    *syscall_return = sys$ioctl(info->ebx, info->ecx, (void *)info->edx);
    break;
  case SYSCALL_MUNMAP:
    *syscall_return = sys$munmap((void *)info->ebx, info->ecx);
    break;
  case SYSCALL_MODIFY_LDT:
    // TODO: sys$set_modify_ldt is not implemnted.
    break;
  case SYSCALL_MPROTECT:
    // TODO: sys$mprotect is not implemented.
    *syscall_return = 0;
    break;
  case SYSCALL_WRITEV:
    *syscall_return = sys$writev(info->ebx, (const iovec *)info->ecx, info->edx);
    break;
  case SYSCALL_MMAP:
  case SYSCALL_MMAP2:
    *syscall_return = (u32)sys$mmap((void *)info->ebx, info->ecx, info->edx, info->esi, info->edi, info->ebp);
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
    serial_port_printf(COM1, "Syscall number '%d' doesn't exist. Arguments: %d, %d, %d, %d, %d, %d\n", syscall_number, info->ebx, info->ecx, info->edx, info->esi, info->edi, info->ebp);
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

#define MAP_PRIVATE 0x2
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20

void *sys$mmap(void *address, size_t size, int protection, int flags, int fd, u32 page_offset)
{
  (void)address;
  (void)protection;

  //TODO: ASSERT(address == NULL);
  //TODO: ASSERT(protection == 0);
  ASSERT(flags <= (MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED));
  ASSERT(fd == -1);
  ASSERT(page_offset == 0);

  // TODO: Huge hack, physical page allocator should be able to allocated multiple pages at a specific address
  addr start_address = allocate_physical_page();
  addr last_address = start_address;

  for (u32 i = 1; i < divide_and_round_up(size, PAGE_SIZE); i++)
  {
    addr page_address = allocate_physical_page();

    ASSERT(page_address - last_address == PAGE_SIZE);
    last_address = page_address;
  }

  return (void *)start_address;
}

int sys$brk(void *addr)
{
  if (addr != NULL)
  {
    get_current_executable()->program_break = (u32)addr;
  }

  return get_current_executable()->program_break;
}

#define TIOCGWINSZ 0x5413

int sys$ioctl(int fd, u32 request, void *arg)
{
  (void)arg;

  if (fd == 1 && request == TIOCGWINSZ)
  {
    return 0;
  }

  serial_port_printf(COM1, "Unimplemented ioctl request: %d\n", request);
  ASSERT_ALWAYS("");
}

int sys$munmap(void *address, size_t length)
{
  size_t page_length = divide_and_round_up(length, PAGE_SIZE);

  for (size_t i = 0; i < page_length; i++)
  {
    free_physical_page((u32)address + i * PAGE_SIZE);
  }

  return 0;
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
