#include "pic.h"
#include "io.h"

void program_pics()
{
  u8 pic1_mask, pic2_mask;

  pic1_mask = in8(PIC1_DATA);
  pic2_mask = in8(PIC2_DATA);

  out8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // Start the initialization sequence (in cascade mode)
  out8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  out8(PIC1_DATA, PIC1_OFFSET); // ICW2: Master PIC vector offset
  out8(PIC2_DATA, PIC2_OFFSET); // ICW2: Slave PIC vector offset
  out8(PIC1_DATA, 4);           // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  out8(PIC2_DATA, 2);           // ICW3: tell Slave PIC its cascade identity (0000 0010)

  out8(PIC1_DATA, ICW4_8086);
  out8(PIC2_DATA, ICW4_8086);

  out8(PIC1_DATA, pic1_mask); // restore saved masks.
  out8(PIC2_DATA, pic2_mask);

  // TEMP HACK: Disable all interrupts except keyboard for the time being.
  out8(PIC1_DATA, 0xfd);
  out8(PIC2_DATA, 0xff);

  asm volatile("sti");
}

void pic_send_eoi(u8 irq)
{
  if (irq >= 8)
  {
    out8(PIC2_COMMAND, PIC_EOI);
  }

  out8(PIC1_COMMAND, PIC_EOI);
}