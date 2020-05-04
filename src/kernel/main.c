#include <stdio.h>
#include <libk/assert.h>
#include "memory_manager.h"
#include "multiboot.h"
#include "keyboard.h"
#include "serial_port.h"
#include "screen.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "kmalloc.h"
#include "ata.h"
#include "epicfs.h"
#include "syscall.h"

void kernel_main(u32 multiboot_magic, multiboot_info_t *multiboot)
{
  memory_manager_init(multiboot);
  kmalloc_init();

  serial_port_init(COM1);

  ASSERT(multiboot_magic == MULTIBOOT_BOOTLOADER_MAGIC);

  program_pics();

  gdt_install();
  idt_install();

  syscall_init();

  keyboard_init();
  screen_init();

  ata_init();

  terminal_init();

  serial_port_printf(COM1, "Hello, world!\n");

  fs_entry *root = epicfs_parse_from_ata(0);

  epicfs_pretty_print_directory_entry(*root, "", 0);

  serial_port_printf(COM1, "/file1 contents: %s\n", epicfs_read_file("/file1", root));

  while (1)
    ;
}