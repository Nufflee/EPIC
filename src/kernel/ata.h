#pragma once

#include <stddef.h>

#define SECTOR_SIZE 512

void ata_init();
u8 *ata_read_sector(u32 offset);
u8 *ata_read(size_t sector_offset, size_t length);