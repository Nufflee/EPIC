#pragma once

#include <stddef.h>

#define SECTOR_SIZE 512

void ata_init();
u8 *ata_read_sector(size_t offset);
u8 *ata_read(size_t sector_offset, size_t length);
void ata_read_into(size_t sector_offset, size_t length, u8 *buffer);