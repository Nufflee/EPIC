#include <stddef.h>
#include <stdio.h>
#include "printf.h"
#include "keyboard.h"
#include "serial_port.h"
#include "io.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"

/*
 * 1. Keyboard input
 * 2. Clean up code
 * 3. Serial port
 */

void kernel_main()
{
  terminal_clear();

  serial_port_init(COM1);

  printf("Hello, world!\n");
  serial_port_printf(COM1, "Hello, world!\n");

  gdt_install();
  idt_install();

  while (1)
    ;
  //keyboard_start_polling();
}