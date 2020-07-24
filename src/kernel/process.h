#pragma once

#include <stddef.h>
#include <libk/string.h>
#include "executable.h"

int process_execute(char *path, u32 argc, string *argv, int *return_code);

executable *get_current_executable();