#include "gdt.h"

// Actual GDT with 5 entries.
static gdt_entry gdt[5];
// Special pointer to the GDT. Later passed to ASM.
static gdt_ptr gp;

static void gdt_set_entry(int num, u32 base, u32 limit, u8 access, u8 granularity)
{
  gdt[num].base_low = (base & 0xFFFF);
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;

  gdt[num].size_low = (limit & 0xFFFF);
  gdt[num].granularity = ((limit >> 16) & 0x0F);

  gdt[num].granularity |= (granularity & 0xF0);
  gdt[num].access = access;
}

void gdt_install()
{
  gp.size = sizeof(gdt_entry) * 5 - 1;
  gp.base = (u32)&gdt;

  // The NULL descriptor
  gdt_set_entry(0, 0, 0, 0, 0);

  /* The second entry is our Code Segment.  The base address
	 * is 0, the limit is 4 gigabytes, it uses 4 kilobyte
	 * granularity, uses 32-bit opcodes, and is a Code Segment
	 * descriptor.  Please check the table above in the tutorial
	 * in order to see exactly what each value means */
  gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

  /* The third entry is our Data Segment.  It's exactly the
	 * same as our code segment, but the descriptor type in
	 * this entry's access byte says it's a Data Segment */
  gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

  // Install the user mode segments into the GDT
  gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
  gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

  // Flush the old GDT and install the new one!
  gdt_flush((u32)&gp);
}