#pragma once

typedef enum syscall_type
{
  SYSCALL_WRITE = 4
};

void syscall_init();