#pragma once

#include <stddef.h>
#include <libk/string.h>

#define PROCESS_BASE_ADDRESS 0x90000

int process_execute(char *path, u32 argc, string *argv, int *return_code);