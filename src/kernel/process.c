#include <string.h>
#include "process.h"
#include "epicfs.h"

addr original_esp;
addr original_ebp;

// Stack has to be aligned to 16 bytes per System V i386 ABI
u8 process_stack[8192] __attribute__((aligned(16)));

int process_execute(char *path, u32 argc, string *argv, int *return_code)
{
  if (epicfs_read_file(path, (u8 *)PROCESS_BASE_ADDRESS) < 0)
  {
    return -1;
  }

  // Save registers so they can be restored after process exits
  asm volatile("pusha");

  // Save 'esp' and 'ebp' so stack can be restored after process exits
  asm volatile("movl %%esp, %0;"
               "movl %%ebp, %1;"
               : "=r"(original_esp),
                 "=r"(original_ebp));

  // Set up a new stack for the process (userspace stack)
  asm volatile("movl %0, %%esp"
               :
               : "r"(process_stack + sizeof(process_stack)));

  for (int i = argc - 1; i >= 0; i--)
  {
    asm volatile("push %0" ::"r"(argv[i]));
  }

  asm volatile("push %0" ::"r"(argc));

  // Set up the 'ebp' for the process' stack
  asm volatile("movl %esp, %ebp;");

  // Jump to the binary to start execution
  asm volatile("jmp *%0"
               :
               : "r"(PROCESS_BASE_ADDRESS));

  // Label to which sys$exit syscall jumps when program exits to return to the kernel
  asm volatile(".global process_kernel_return;"
               "process_kernel_return:");

  // Restore kernel's stack
  asm volatile("movl %0, %%esp;"
               "movl %1, %%ebp;"
               :
               : "m"(original_esp),
                 "m"(original_ebp));

  if (return_code)
  {
    // sys$exit syscall puts the return code into 'eax'
    asm volatile("movl %%eax, %0"
                 : "=r"(*return_code));
  }

  // Restore registers
  asm volatile("popa");

  // Clear the stack so the next program has a fresh stack to work with
  memset(process_stack, 0, sizeof(process_stack));

  return 0;
}