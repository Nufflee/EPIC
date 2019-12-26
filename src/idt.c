#include "idt.h"
#include "serial_port.h"
#include "interrupts.h"

static idt_entry idt[256];
static idt_ptr ip;

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

  idt_set_entry(0x3, (u32)isr3, 0x08, 0x8E);

  idt_flush((u32)&ip);
}