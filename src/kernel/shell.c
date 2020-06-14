#include <stdio.h>
#include <libk/string.h>
#include "shell.h"
#include "kmalloc.h"
#include "serial_port.h"
#include "process.h"

void shell_execute_command(char *command)
{
  string *parts = string_split(command, ' ');

  int argc = 0;

  while (*(parts + argc) != NULL)
  {
    argc++;
  }

  if (process_execute(parts[0], argc, parts) < 0)
  {
    printf("'%s' is not recognized as an internal or external command, operable program or batch file!\n", parts[0]);
  }

  kfree(parts);

  while (*parts != NULL)
  {
    kfree(*(parts++));
  }
}