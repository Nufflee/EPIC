#pragma once

#include <stddef.h>

#define CHUNK_SIZE sizeof(size_t)

typedef struct allocation_header
{
  size_t chunk_size;
  u32 start_chunk;
  u8 checksum;
} allocation_header;

void kmalloc_init();

void *kmalloc(size_t size);
void *kalloc(size_t element_size, size_t length);
void kfree(void *address);