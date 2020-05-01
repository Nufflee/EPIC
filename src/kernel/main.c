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

void kernel_main(u32 multiboot_magic, multiboot_info_t *multiboot)
{
  memory_manager_init(multiboot);
  kmalloc_init();

  serial_port_init(COM1);

  ASSERT(multiboot_magic == MULTIBOOT_BOOTLOADER_MAGIC);

  program_pics();

  gdt_install();
  idt_install();

  keyboard_init();

  screen_init();

  ata_init();

  terminal_init();

  serial_port_printf(COM1, "Hello, world!\n");

  epicfs_pretty_print_directory_entry(*epicfs_parse_from_ata(0), "", 0);

  while (1)
    ;
}