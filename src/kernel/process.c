#include <string.h>
#include <libk/assert.h>
#include "process.h"
#include "epicfs.h"
#include "elf.h"
#include "serial_port.h"
#include "kmalloc.h"
#include "executable.h"

// It's important that these are not stored on the stack
addr original_esp;
addr original_ebp;

u8 process_stack[8192] __attribute__((aligned(16)));

static inline void push_aux_vectors();

int process_execute(char *path, u32 argc, string *argv, int *return_code)
{
  executable exec;

  if (executable_load(path, &exec) < 0)
  {
    return -1;
  }

  // Save registers so they can be restored after process exits
  asm volatile("pusha;");

  // Save 'esp' and 'ebp' so stack can be restored after process exits
  asm volatile(
      "movl %%esp, %0;"
      "movl %%ebp, %1;"
      : "=m"(original_esp),
        "=m"(original_ebp));

  // Set up a new stack for the process (userspace stack)
  asm volatile("movl %0, %%esp" ::"r"(process_stack + sizeof(process_stack)));

  if (exec.type == ELF32_EXECUTABLE)
  {
    push_aux_vectors();
  }

  // 'envp' NULL terminator
  asm volatile("push $0;");

  // 'argv' NULL terminator
  asm volatile("push $0;");

  // TODO: Bake these directly into `process_stack` image instead of using asm
  for (int i = argc - 1; i >= 0; i--)
  {
    asm volatile("push %0;" ::"m"(argv[i]));
  }

  asm volatile("push %0;" ::"m"(argc));

  asm volatile("movl %0, %%eax;"    // Save entry_point before switching stack frames
               "movl %%esp, %%ebp;" // Set up the 'ebp' for the process' stack
               "jmp *%%eax;"        // Jump to the binary to start execution
               :
               : "m"(exec.entry_point));

  // Label to which sys$exit syscall jumps when program exits to return to the kernel
  asm volatile(".global process_kernel_return;"
               "process_kernel_return:");

  // Restore kernel's stack
  asm volatile(
      "movl %0, %%esp;"
      "movl %1, %%ebp;"
      :
      : "m"(original_esp),
        "m"(original_ebp));

  if (return_code)
  {
    // sys$exit syscall puts the return code into 'eax'
    asm volatile("movl %%eax, %0;"
                 : "=r"(*return_code));
  }

  // Restore registers
  asm volatile("popa;");

  // Clear the stack so the next program has a fresh stack to work with
  memset(process_stack, 0, sizeof(process_stack));

  return 0;
}

// Always has to be inlined because things it pushes to the stack cannot be dismissed
__attribute__((always_inline)) static inline void push_auxv(Aux32Info auxv)
{
  asm volatile("push %0;"
               "push %1;"
               :
               : "m"(auxv.value),
                 "m"(auxv.type));
}

__attribute__((always_inline)) static inline void push_aux_vectors()
{
  // TODO: This is a placeholder vDSO address entry
  Aux32Info vdso_auxv = {
      .type = 33,
      .value = 0x6969,
  };

  Aux32Info null_auxv = {
      .type = AUX_null,
      .value = 0x0,
  };

  push_auxv(null_auxv);
  push_auxv(vdso_auxv);
}