#include <sys/auxv.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  printf("Hello gamers!\n");

  printf("getauxval(AT_SYSINFO_EHDR) = 0x%lx\n", getauxval(AT_SYSINFO_EHDR));

  printf("argc = %d\n", argc);

  printf("argv[0] = %s\n", argv[0]);

  if (argc == 2)
  {
    printf("argv[1] = %s\n", argv[1]);
  }

  return 69;
}
