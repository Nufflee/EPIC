#include <stddef.h>

#define EXECUTABLE_BASE_ADDRESS 0x90000

typedef enum executable_type
{
  ELF32_EXECUTABLE,
  FLAT_EXECUTABLE
} executable_type;

typedef struct executable
{
  addr entry_point;
  addr program_break;
  executable_type type;
} executable;

int executable_load(char *path, executable *exec);