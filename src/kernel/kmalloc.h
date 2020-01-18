#pragma once

#include <stddef.h>

typedef struct allocation_node
{
  size_t chunk_size;
  u32 start_chunk;
  u16 checksum;
  u8 data[];
} __attribute__((packed)) allocation_node;

void kmalloc_init();

void *kmalloc(size_t size);
void kfree(addr address);