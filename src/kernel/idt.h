#pragma once

#include <stddef.h>

/* Single IDT entry
 * Reference: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html */
typedef struct idt_entry
{
  u16 jump_to_low;
  u16 code_segment_selector;
  u8 always0;
  u8 flags;
  u16 jump_to_high;
} __attribute__((packed)) idt_entry;

// Special pointer to the whole IDT.
typedef struct idt_ptr
{
  u16 size;
  u32 base;
} __attribute__((packed)) idt_ptr;

// Defined in boot.s.
extern void idt_flush(u32 idt_ptr);

void idt_install();