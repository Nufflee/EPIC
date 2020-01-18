#pragma once

#include <stddef.h>
#include "multiboot.h"

#define PAGE_SIZE 4096

void memory_manager_init(multiboot_info_t *multiboot);

u32 memory_manager_get_available_memory();

u32 allocate_physical_page();
void free_physical_page(u32 page);