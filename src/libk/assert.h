#pragma once

#ifndef RELEASE
#include <stdbool.h>
#include <kernel/serial_port.h>

#define ASSERT_MESSAGE(condition, message)                                                     \
  do                                                                                           \
  {                                                                                            \
    if (!(condition))                                                                          \
    {                                                                                          \
      serial_port_printf(COM1, "[%s:%d] %s: %s\n", __FILE__, __LINE__, __FUNCTION__, message); \
                                                                                               \
      asm volatile("cli");                                                                     \
                                                                                               \
      while (true)                                                                             \
      {                                                                                        \
      }                                                                                        \
    }                                                                                          \
  } while (false)

#define ASSERT(condition) ASSERT_MESSAGE(condition, "Assert '" #condition "' failed!")
#define ASSERT_ALWAYS(message) ASSERT_MESSAGE(false, message)
#define ASSERT_NOT_REACHED() ASSERT_MESSAGE(false, "Assert shouldn't have been reached!")
#else
#define ASSERT_MESSAGE(condition, message)
#define ASSERT(condition)
#define ASSERT_ALWAYS(message)
#define ASSERT_NOT_REACHED()
#endif