#include <stdbool.h>
#include <libk/assert.h>
#include <libk/math.h>
#include <libk/bits.h>
#include "kmalloc.h"
#include "memory_manager.h"

#define KMALLOC_DEBUG

#define CHUNK_SIZE sizeof(size_t)

static u32 CHUNK_COUNT;

static u8 *pool;
static addr *pages;

static size_t allocated_chunk_count;

static void *kalloc_eternal(size_t, size_t);
static u8 calculate_allocation_checksum(allocation_header *);

void kmalloc_init()
{
  u32 memory_size = memory_manager_get_available_memory();

  CHUNK_COUNT = memory_size / CHUNK_SIZE;

  pool = kalloc_eternal(sizeof(u8), CHUNK_COUNT / 8);
  pages = kalloc_eternal(sizeof(addr), CHUNK_COUNT / (PAGE_SIZE / CHUNK_SIZE));
}

void *kmalloc(size_t size)
{
  ASSERT(size > 0);

  size += sizeof(allocation_header);

  size_t chunks_to_allocate = divide_and_round_up(size, CHUNK_SIZE);

  for (size_t start_chunk_number = 0; start_chunk_number + chunks_to_allocate < CHUNK_COUNT; start_chunk_number++)
  {
    bool sequence_found = true;

    for (size_t i = 0; i < chunks_to_allocate; i++)
    {
      u32 chunk_number = start_chunk_number + i;
      u32 pool_index = chunk_number / 8;
      u32 pool_bit = chunk_number % 8;

      if (BIT_GET(pool[pool_index], pool_bit) != 0)
      {
        sequence_found = false;

        if (pool[pool_index] == 0xff)
        {
          start_chunk_number += 7;
        }
        else
        {
          start_chunk_number = chunk_number;
        }

        break;
      }
    }

    if (sequence_found)
    {
      for (size_t chunk_number = start_chunk_number; chunk_number < start_chunk_number + chunks_to_allocate; chunk_number++)
      {
        u32 pool_index = chunk_number / 8;
        u32 page_index = chunk_number / (PAGE_SIZE / CHUNK_SIZE);

        if (pages[page_index] == 0)
        {
          pages[page_index] = allocate_physical_page();

          serial_port_printf(COM1, "kmalloc: Allocated new physical page at %#x\n", pages[page_index]);
        }

        BIT_SET(pool[pool_index], chunk_number % 8);
      }

      u32 start_page_index = start_chunk_number / PAGE_SIZE;

      addr addr = pages[start_page_index] + (start_chunk_number - start_page_index * PAGE_SIZE) * CHUNK_SIZE;
      allocation_header *header = (allocation_header *)addr;

      memset(header, 0, sizeof(allocation_header));

      header->start_chunk = start_chunk_number;
      header->chunk_size = divide_and_round_up((size - sizeof(allocation_header)), CHUNK_SIZE);
      header->checksum = calculate_allocation_checksum(header);

      allocated_chunk_count += header->chunk_size;

#ifdef KMALLOC_DEBUG
      serial_port_printf(COM1, "kmalloc: Allocated %d chunks (%d bytes) at %#x (chunk %d) with checksum %d.\n", header->chunk_size, header->chunk_size * CHUNK_SIZE, (addr + sizeof(allocation_header)), header->start_chunk, header->checksum);
#endif

      return (void *)(header + 1);
    }
  }

  ASSERT_ALWAYS("Couldn't find a contigous sequence of chunks for this allocation!");
}

void *kalloc(size_t element_size, size_t length)
{
  return kmalloc(element_size * length);
}

void kfree(void *address)
{
  allocation_header *header = (allocation_header *)(address - sizeof(allocation_header));

  allocated_chunk_count -= header->chunk_size;

#ifdef KMALLOC_DEBUG
  serial_port_printf(COM1, "kfree: Freeing %d chunks (%d bytes) at %#x (chunk %d) with checksum %d. %d chunks still allocated.\n", header->chunk_size, header->chunk_size * CHUNK_SIZE, address, header->start_chunk, header->checksum, allocated_chunk_count);
#endif

  ASSERT(!calculate_allocation_checksum(header));

  for (size_t i = header->start_chunk; i < header->start_chunk + header->chunk_size + divide_and_round_up(sizeof(allocation_header), CHUNK_SIZE); i++)
  {
    u16 chunk_index = i / 8;
    u8 chunk_bit = i % 8;

    ASSERT(BIT_GET(pool[chunk_index], chunk_bit) == true);

    BIT_CLEAR(pool[chunk_index], chunk_bit);
  }

  for (size_t i = header->start_chunk / PAGE_SIZE; i <= (header->start_chunk + header->chunk_size) / PAGE_SIZE; i++)
  {
    const u32 bytes_per_page = PAGE_SIZE / CHUNK_SIZE / 8;
    bool can_free = true;

    for (size_t j = i * bytes_per_page; j < (i * bytes_per_page) + bytes_per_page; j++)
    {
      if (pool[j] != 0)
      {
        can_free = false;

        break;
      }
    }

    if (can_free)
    {
      free_physical_page(pages[i]);

      serial_port_printf(COM1, "kfree: Free'd physical page at %#x\n", pages[i]);

      pages[i] = 0;
    }
  }
}

// This should never be used after kmalloc_init!
static void *kalloc_eternal(size_t element_size, size_t length)
{
  static addr eternal_address = 0;

  ASSERT(element_size > 0 && length > 0);

  if (eternal_address == 0)
  {
    eternal_address = allocate_physical_page();
  }

  addr result = eternal_address;
  size_t size = element_size * length;
  u32 pages_to_allocate = (eternal_address + size) / PAGE_SIZE - eternal_address / PAGE_SIZE;

  for (size_t page = 0; page < pages_to_allocate; page++)
  {
    allocate_physical_page();
  }

  memset((addr *)eternal_address, 0, size);

  eternal_address += size;

  serial_port_printf(COM1, "kalloc_eternal: Allocated %d pages for %d bytes\n", pages_to_allocate, size);

  return (void *)result;
}

static u8 calculate_allocation_checksum(allocation_header *header)
{
  u8 checksum = 0;

  for (size_t i = 0; i < sizeof(allocation_header); i++)
  {
    checksum = (checksum + ((u8 *)header)[i]) & 0xFF;
  }

  return ((checksum ^ 0xFF) + 1) & 0xFF;
}