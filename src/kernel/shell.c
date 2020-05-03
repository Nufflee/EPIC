#include <stdio.h>
#include <libk/string.h>
#include "shell.h"
#include "kmalloc.h"
#include "serial_port.h"

void shell_execute_command(char *command)
{
  string *parts = string_split(command, " ");

  printf("'%s' command not found!\n", parts[0]);

  kfree(parts);

  while (*parts != NULL)
  {
    kfree(*(parts++));
  }
}