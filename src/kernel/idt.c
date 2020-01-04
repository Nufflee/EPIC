#include "idt.h"
#include "serial_port.h"
#include "interrupts.h"

static idt_entry idt[256];
static idt_ptr ip;

#define MAKE_IDT_ENTRY(x) idt_set_entry(x, (u32)isr##x, 0x08, 0x8E)

static void idt_set_entry(u8 number, u32 base, u16 segment_selector, u8 flags)
{
  idt[number].jump_to_low = base & 0xFFFF;
  idt[number].jump_to_high = (base >> 16) & 0xFFFF;

  idt[number].code_segment_selector = segment_selector;
  idt[number].always0 = 0;
  idt[number].flags = flags /* | 0x60 */;
}

void idt_install()
{
  ip.size = sizeof(idt_entry) * 256 - 1;
  ip.base = (u32)&idt;

  memset(&idt, 0, sizeof(idt_entry) * 256);

  MAKE_IDT_ENTRY(1);
  MAKE_IDT_ENTRY(2);
  MAKE_IDT_ENTRY(3);
  MAKE_IDT_ENTRY(4);
  MAKE_IDT_ENTRY(5);
  MAKE_IDT_ENTRY(6);
  MAKE_IDT_ENTRY(7);
  MAKE_IDT_ENTRY(8);
  MAKE_IDT_ENTRY(9);
  MAKE_IDT_ENTRY(10);
  MAKE_IDT_ENTRY(11);
  MAKE_IDT_ENTRY(12);
  MAKE_IDT_ENTRY(13);
  MAKE_IDT_ENTRY(14);
  MAKE_IDT_ENTRY(15);
  MAKE_IDT_ENTRY(16);
  MAKE_IDT_ENTRY(17);
  MAKE_IDT_ENTRY(18);
  MAKE_IDT_ENTRY(19);
  MAKE_IDT_ENTRY(20);
  MAKE_IDT_ENTRY(21);
  MAKE_IDT_ENTRY(22);
  MAKE_IDT_ENTRY(23);
  MAKE_IDT_ENTRY(24);
  MAKE_IDT_ENTRY(25);
  MAKE_IDT_ENTRY(26);
  MAKE_IDT_ENTRY(27);
  MAKE_IDT_ENTRY(28);
  MAKE_IDT_ENTRY(29);
  MAKE_IDT_ENTRY(30);
  MAKE_IDT_ENTRY(31);

  idt_set_entry(33, (u32)isr33, 0x08, 0x8E);

  idt_flush((u32)&ip);
}