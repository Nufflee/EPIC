#pragma once

#include <stddef.h>

#define PROCESS_BASE_ADDRESS 0x90000

void process_execute(char *path);