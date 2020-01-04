#pragma once

#include <stddef.h>

size_t strlen(const char *string);

void *memset(void *buffer, int value, size_t length);
void *memcpy(void *destination, const void *source, size_t length);