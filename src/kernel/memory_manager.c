#include <libk/assert.h>
#include "memory_manager.h"

extern u32 KERNEL_END;

static u8 *page_bitmap;

static u32 base_address = 0x400000; // ((u32)&KERNEL_END + 0x100000);
static u32 memory_end;
static u32 memory_page_count;

#define BIT_GET(value, bit) ({ typeof(bit) bit_ = (bit); ((value & (1 << bit_)) >> bit_); })
#define BIT_SET(value, bit) value |= 1 << bit
#define BIT_CLEAR(value, bit) value &= ~(1 << bit)

void memory_manager_init(multiboot_info_t *multiboot)
{
  multiboot_memory_map_t *entry = (multiboot_memory_map_t *)multiboot->mmap_addr;

  while ((u32)entry < multiboot->mmap_addr + multiboot->mmap_length)
  {
    entry = (multiboot_memory_map_t *)((u32)entry + entry->size + sizeof(entry->size));

    if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
    {
      memory_end = (entry->addr + entry->len);

      break;
    }
  }

  ASSERT(memory_end > 0 && "No available memory regions found!");

  u32 available_memory = memory_end - base_address;
  memory_page_count = available_memory / PAGE_SIZE;

  memset((u32 *)base_address, 0, memory_page_count / 8);

  page_bitmap = (u8 *)base_address;
}

u32 allocate_physical_page()
{
  u32 bitmap_length = memory_page_count / 8;

  for (u32 i = 0; i < bitmap_length; i++)
  {
    u8 page_byte = page_bitmap[i];

    for (u8 j = 0; j < 8; j++)
    {
      if (BIT_GET(page_byte, j) == 0)
      {
        BIT_SET(page_bitmap[i], j);

        u32 address = (bitmap_length + base_address) + ((i * 8) + j) * PAGE_SIZE;

        return address;
      }
    }
  }

  ASSERT(false && "Ran out of memory!");
}

void free_physical_page(u32 address)
{
  ASSERT(address <= memory_end);

  u32 page_number = (address - base_address) / PAGE_SIZE;
  u32 byte_index = page_number / 8;
  u8 bit_index = page_number % 8;
  bool is_page_allocated = BIT_GET(page_bitmap[byte_index], bit_index);

  ASSERT(is_page_allocated == true);

  BIT_CLEAR(page_bitmap[byte_index], bit_index);
}