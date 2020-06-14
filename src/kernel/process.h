#pragma once

#include <stddef.h>
#include <libk/string.h>

#define PROCESS_BASE_ADDRESS 0x90000

int process_execute(char *path, uint32_t argc, string *argv);