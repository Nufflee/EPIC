#include <libk/assert.h>
#include "elf.h"
#include "executable.h"
#include "serial_port.h"
#include "epicfs.h"

int flat_binary_load(char *, executable *);
int elf32_load(char *, Elf32_Ehdr *, executable *);
void elf32_print_phdr(Elf32_Phdr *);

static u8 buffer[150 * 1024];

int executable_load(char *path, executable *exec)
{
  // TODO: Only load the ELF32 File Header
  if (epicfs_read_file(path, buffer) < 0)
  {
    return -1;
  }

  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)buffer;

  if (IS_ELF(*ehdr))
  {
    return elf32_load(path, ehdr, exec);
  }
  else
  {
    return flat_binary_load(path, exec);
  }

  return 0;
}

int flat_binary_load(char *path, executable *exec)
{
  exec->type = FLAT_EXECUTABLE;

  if (epicfs_read_file(path, (u8 *)EXECUTABLE_BASE_ADDRESS) < 0)
  {
    return -1;
  }

  exec->entry_point = EXECUTABLE_BASE_ADDRESS;

  return 0;
}

int elf32_load(char *path, Elf32_Ehdr *ehdr, executable *exec)
{
  (void)path;

  ASSERT(IS_ELF(*ehdr));
  ASSERT(ehdr->e_ident[EI_CLASS] == ELF_TARG_CLASS);
  ASSERT(ehdr->e_type == ET_EXEC);
  ASSERT(ehdr->e_machine == ELF_TARG_MACH);

  exec->type = ELF32_EXECUTABLE;

  for (int i = 0; i < ehdr->e_phnum; i++)
  {
    Elf32_Phdr *phdr = (Elf32_Phdr *)(buffer + ehdr->e_phoff + ehdr->e_phentsize * i);

#ifdef ELF32_DEBUG
    elf32_print_phdr(phdr);
#endif

    if (phdr->p_type != PT_LOAD)
    {
      continue;
    }

    memcpy((void *)phdr->p_paddr, buffer + phdr->p_offset, phdr->p_filesz);

    // If p_memsz != p_filesz, there's a 'nobits' section which means that we have to properly expand it and zero initialize it
    memset((void *)(phdr->p_paddr + phdr->p_filesz), 0, (phdr->p_memsz - phdr->p_filesz));

    exec->program_break = phdr->p_paddr + phdr->p_memsz;
  }

  exec->entry_point = ehdr->e_entry;

  return 0;
}

void elf32_print_phdr(Elf32_Phdr *phdr)
{
  serial_port_printf(COM1, "\t");

  if (phdr->p_type == PT_NULL)
  {
    serial_port_printf(COM1, "NULL ");
  }
  else if (phdr->p_type == PT_LOAD)
  {
    serial_port_printf(COM1, "LOAD ");
  }
  else
  {
    serial_port_printf(COM1, "UNKNOWN (%d) ", phdr->p_type);
  }

  if (phdr->p_flags & PF_R)
  {
    serial_port_printf(COM1, "R");
  }
  else
  {
    serial_port_printf(COM1, " ");
  }

  if (phdr->p_flags & PF_X)
  {
    serial_port_printf(COM1, "E");
  }
  else
  {
    serial_port_printf(COM1, " ");
  }

  if (phdr->p_flags & PF_W)
  {
    serial_port_printf(COM1, "W");
  }
  else
  {
    serial_port_printf(COM1, " ");
  }

  serial_port_printf(COM1, " ");

  serial_port_printf(COM1, "0x%x ", phdr->p_paddr);

  serial_port_printf(COM1, "0x%x\n", phdr->p_offset);
}