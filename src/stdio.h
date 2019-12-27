#include <stddef.h>
#include <stdarg.h>

#define BUFFER_LEN 2048

int printf(const char *format, ...);

int sprintf(char *buffer, const char *format, ...);
int vsprintf(char *buffer, const char *format, va_list list);
int snprintf(char *buffer, size_t length, const char *format, ...);
int vsnprintf(char *buffer, size_t length, const char *format, va_list list);