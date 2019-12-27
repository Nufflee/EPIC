#include <stddef.h>
#include <stdio.h>
#include "printf.h"
#include "keyboard.h"
#include "serial_port.h"
#include "io.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"

void kernel_main()
{
  terminal_clear();

  serial_port_init(COM1);

  printf("Hello, world!\n");
  serial_port_printf(COM1, "Hello, world!\n");

  program_pics();

  gdt_install();
  idt_install();

  keyboard_init();

  while (1)
    ;
}