#include <stddef.h>
#include <stdio.h>
#include "printf.h"
#include "keyboard.h"
#include "serial_port.h"
#include "io.h"
#include "terminal.h"

/*
 * 1. Keyboard input
 * 2. Clean up code
 * 3. Serial port
 */

#define INT_DISABLE 0
#define INT_ENABLE 0x200
#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

void init_pics(int pic1, int pic2)
{
  /* send ICW1 */
  out8(PIC1, ICW1);
  out8(PIC2, ICW1);

  /* send ICW2 */
  out8(PIC1 + 1, pic1);
  out8(PIC2 + 1, pic2);

  /* send ICW3 */
  out8(PIC1 + 1, 4);
  out8(PIC2 + 1, 2);

  /* send ICW4 */
  out8(PIC1 + 1, ICW4);
  out8(PIC2 + 1, ICW4);

  /* disable all IRQs */
  out8(PIC1 + 1, 0xFF);

  asm volatile("cli");
}

void kernel_main()
{
  init_pics(0x20, 0x28);

  terminal_clear();

  serial_port_init(COM1);

  printf("Hello, world!\n");
  serial_port_printf(COM1, "Hello, world!\n");

  keyboard_start_polling();
}