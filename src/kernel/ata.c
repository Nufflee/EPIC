#include "ata.h"
#include <libk/string.h>
#include <libk/math.h>
#include "io.h"
#include "serial_port.h"
#include "ata_defs.h"
#include "kmalloc.h"
#include "terminal.h"

u8 drive_info_buffer[SECTOR_SIZE];

static void ata_400ns_delay();
static void ata_poll();

void ata_init()
{
  u16 io = ATA_PRIMARY_IO;

  // Select primary bus master drive.
  out8(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);

  // ATA specs say these values must be zero before sending IDENTIFY
  out8(io + ATA_REG_SECCOUNT0, 0);
  out8(io + ATA_REG_LBA0, 0);
  out8(io + ATA_REG_LBA1, 0);
  out8(io + ATA_REG_LBA2, 0);

  out8(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

  u8 status = in8(io + ATA_REG_STATUS);

  if (status)
  {
    ata_poll();

    serial_port_printf(COM1, "Drive is online.\n");

    for (int i = 0; i < SECTOR_SIZE / 2; i++)
    {
      *(uint16_t *)(drive_info_buffer + i * 2) = in16(io + ATA_REG_DATA);
    }

    char name[40];

    for (int i = 0; i < 40; i += 2)
    {
      name[i] = drive_info_buffer[ATA_IDENT_MODEL + i + 1];
      name[i + 1] = drive_info_buffer[ATA_IDENT_MODEL + i];
    }

    serial_port_printf(COM1, "Drive name: %s\n", name);
  }
}

u8 *ata_read_sector(size_t sector_offset)
{
  u8 *buffer = kmalloc(SECTOR_SIZE);

  u8 cmd = 0xE0;

  u16 io = ATA_PRIMARY_IO;

  out8(io + ATA_REG_HDDEVSEL, (cmd | (u8)((sector_offset >> 24 & 0x0F))));
  out8(io + 1, 0x00);
  out8(io + ATA_REG_SECCOUNT0, 1);

  out8(io + ATA_REG_LBA0, (uint8_t)((sector_offset)));
  out8(io + ATA_REG_LBA1, (uint8_t)((sector_offset >> 8)));
  out8(io + ATA_REG_LBA2, (uint8_t)((sector_offset) >> 16));

  out8(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  ata_poll();

  for (int i = 0; i < SECTOR_SIZE / 2; i++)
  {
    uint16_t data = in16(io + ATA_REG_DATA);
    *(uint16_t *)(buffer + i * 2) = data;
  }

  ata_400ns_delay(io);

  return buffer;
}

u8 *ata_read(size_t sector_offset, size_t length)
{
  u8 *buffer = kmalloc(length);
  size_t bytes_left = length;
  size_t i = 0;

  while (bytes_left > 0)
  {
    u8 *sector_buffer = ata_read_sector(sector_offset + i);
    size_t bytes_read = min(SECTOR_SIZE, bytes_left);

    memcpy(buffer + (i * SECTOR_SIZE), sector_buffer, bytes_read);

    bytes_left -= bytes_read;
    i++;

    kfree(sector_buffer);
  }

  return buffer;
}

static void ata_poll()
{
  u16 io = ATA_PRIMARY_IO;

  ata_400ns_delay();

  u8 status;

  while (((status = in8(io + ATA_REG_STATUS)) & ATA_SR_BSY) != 0)
  {
  }

  status = in8(io + ATA_REG_STATUS);

  if (status & ATA_SR_ERR)
  {
    serial_port_printf(COM1, "ERR set, drive failure!\n");
  }

  while (!(in8(io + ATA_REG_STATUS) & ATA_SR_DRQ))
  {
  }
}

static void ata_400ns_delay()
{
  for (int i = 0; i < 4; i++)
  {
    in8(ATA_PRIMARY_IO + ATA_REG_ALTSTATUS);
  }
}