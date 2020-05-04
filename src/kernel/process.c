#include "process.h"
#include "epicfs.h"

void process_execute(char *path)
{
  epicfs_read_file(path, (u8 *)PROCESS_BASE_ADDRESS);

  void (*execute)(void) = (void (*)())PROCESS_BASE_ADDRESS;

  execute();
}