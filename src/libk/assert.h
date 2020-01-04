#pragma once

#ifndef RELEASE
#include <stdbool.h>
#include <kernel/serial_port.h>

#define ASSERT_M(condition, message)                                                         \
  do                                                                                         \
  {                                                                                          \
    if (!(condition))                                                                        \
    {                                                                                        \
      serial_port_printf(COM1, "[%s:%d] %s: %s", __FILE__, __LINE__, __FUNCTION__, message); \
                                                                                             \
      while (true)                                                                           \
      {                                                                                      \
      }                                                                                      \
    }                                                                                        \
  } while (false)

#define ASSERT(condition) ASSERT_M(condition, "Assert '" #condition "' failed!\n")
#define ASSERT_NOT_REACHED() ASSERT_M(false, "Assert shouldn't have been reached!\n")
#else
#define ASSERT_M(condition, message)
#define ASSERT(condition)
#define ASSERT_NOT_REACHED()
#endif