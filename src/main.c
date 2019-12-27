#include <stdio.h>
#include "keyboard.h"
#include "serial_port.h"
#include "screen.h"
#include "shell.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"

void kernel_main()
{
  serial_port_init(COM1);

  program_pics();

  gdt_install();
  idt_install();

  keyboard_init();

  screen_init();

  shell_init();

  printf("Hello, world!\n");
  serial_port_printf(COM1, "Hello, world!\n");

  while (1)
    ;
}