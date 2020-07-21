#include <string.h>
#include <unistd.h>
#include <sys/auxv.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  char buf[100] = {"Hello gamers!\n"};

  write(1, buf, strlen(buf));

  sprintf(buf, "getauxval(AT_SYSINFO_EHDR) = 0x%lx\n", getauxval(AT_SYSINFO_EHDR));
  write(1, buf, strlen(buf));

  sprintf(buf, "argc = %d\n", argc);
  write(1, buf, strlen(buf));

  sprintf(buf, "argv[0] = %s\n", argv[0]);
  write(1, buf, strlen(buf));

  if (argc == 2)
  {
    sprintf(buf, "argv[1] = %s\n", argv[1]);
    write(1, buf, strlen(buf));
  }

  return 69;
}
