#include <stddef.h>

/* Single GDT entry
 * Reference: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html */
typedef struct gdt_entry
{
  u16 size_low;
  u16 base_low;
  u8 base_middle;
  u8 access;
  u8 granularity;
  u8 base_high;
} __attribute__((packed)) gdt_entry;

// Special pointer to the whole GDT. `base` is the start of the GDT and `limit` its size.
typedef struct gdt_ptr
{
  u16 size;
  u32 base;
} __attribute__((packed)) gdt_ptr;

// Defined in boot.s.
extern void gdt_flush(u32 gdt_ptr);

void gdt_install();