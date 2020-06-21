#pragma once

typedef enum syscall_type
{
  SYSCALL_EXIT = 1,
  SYSCALL_WRITE = 4
} syscall_type;

void syscall_init();