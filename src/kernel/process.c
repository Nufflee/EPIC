#include "process.h"
#include "epicfs.h"

int process_execute(char *path, uint32_t argc, string *argv)
{
  for (int i = argc - 1; i >= 0; i--)
  {
    asm volatile("push %0" ::"r"(argv[i]));
  }

  asm volatile("push %0" ::"r"(argc));

  if (epicfs_read_file(path, (u8 *)PROCESS_BASE_ADDRESS) < 0)
  {
    return -1;
  }

  void (*execute)(void) = (void (*)())PROCESS_BASE_ADDRESS;

  execute();

  return 0;
}