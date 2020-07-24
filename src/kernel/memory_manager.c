#include <libk/assert.h>
#include <libk/bits.h>
#include <libk/math.h>
#include "memory_manager.h"

extern u32 KERNEL_END;

static u8 *page_bitmap;
static u32 page_bitmap_length;

static u32 bitmap_base_address = (u32)&KERNEL_END + 0x100;
static u32 memory_end;
static u32 memory_page_count;

static u32 allocator_base_address;

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

  u32 available_memory = memory_end - bitmap_base_address;
  memory_page_count = available_memory / PAGE_SIZE;
  page_bitmap_length = memory_page_count / 8;

  memset((u32 *)bitmap_base_address, 0, page_bitmap_length);
  page_bitmap = (u8 *)bitmap_base_address;

  allocator_base_address = divide_and_round_up(bitmap_base_address + page_bitmap_length, PAGE_SIZE) * PAGE_SIZE;

  serial_port_printf(COM1, "System has %d bytes of memory avilable\n", available_memory);
}

u32 memory_manager_get_available_memory()
{
  return memory_end - allocator_base_address;
}

u32 allocate_physical_page()
{
  for (u32 i = 0; i < page_bitmap_length; i++)
  {
    u8 page_byte = page_bitmap[i];

    for (u8 j = 0; j < 8; j++)
    {
      if (BIT_GET(page_byte, j) == 0)
      {
        BIT_SET(page_bitmap[i], j);

        u32 address = allocator_base_address + ((i * 8) + j) * PAGE_SIZE;

        return address;
      }
    }
  }

  ASSERT_ALWAYS("Ran out of memory!");
}

void free_physical_page(u32 address)
{
  ASSERT(address <= memory_end);

  u32 page_number = (address - allocator_base_address) / PAGE_SIZE;
  u32 byte_index = page_number / 8;
  u8 bit_index = page_number % 8;
  bool is_page_allocated = BIT_GET(page_bitmap[byte_index], bit_index);

  ASSERT(is_page_allocated == true);

  BIT_CLEAR(page_bitmap[byte_index], bit_index);
}