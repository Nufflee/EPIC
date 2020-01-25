#pragma once

#include <stddef.h>

typedef struct allocation_node
{
  size_t chunk_size;
  u32 start_chunk;
  u8 checksum;
} allocation_node;

void kmalloc_init();

void *kmalloc(size_t size);
void kfree(void *address);