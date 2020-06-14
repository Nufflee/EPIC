#include <stdbool.h>
#include <libc/string.h>
#include <libk/assert.h>
#include <kernel/kmalloc.h>
#include "string.h"

string string_concat(string destination, const_string source)
{
  string offset = destination + string_length(destination);

  memcpy(offset, source, string_length(source));
  offset += string_length(source);

  *offset = '\0';

  return destination;
}

int string_compare(const_string str1, const_string str2)
{
  while (*str1 && (*str1 == *str2))
  {
    str1++, str2++;
  }

  return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

// TODO: This breaks when there are multiple consecutive delimiters (and probably in many other cases).
string *string_split(const_string str, char delimiter)
{
  size_t length = string_length(str);
  string *parts = kalloc(length, sizeof(string));
  int count = 0;
  int last = 0;

  for (size_t i = 0; i < length; i++)
  {
    if (str[i] == delimiter)
    {
      parts[count++] = string_substring_range(str, last, i);

      last = i + 1;
    }
  }

  // Make sure to include the last part.
  parts[count++] = string_substring_range(str, last, length);

  parts[count] = NULL;

  return parts;
}

string string_normalize_spaces(const_string str)
{
  string result = kmalloc(string_length(str));
  char c = 0;
  int r = 0;

  for (size_t i = 0; i < string_length(result); i++)
  {
    while (c == ' ') //replaces two or more spaces by a single space
    {
      c = str[i++];
      if (c != ' ')
        result[r++] = c;
    }
  }

  return result;
}

// TODO: Should use a better algorithm here.
int string_find(const_string str, const_string needle)
{
  for (size_t i = 0; i < string_length(str); i++)
  {
    bool found = true;

    for (size_t j = 0; j < string_length(needle); j++)
    {
      if (str[i] != needle[j])
      {
        found = false;

        break;
      }
    }

    if (found)
    {
      return i;
    }
  }

  return -1;
}

string string_substring(const_string str, int start, int length)
{
  ASSERT(length > 0);

  string substr = kmalloc(length + 1);

  memcpy(substr, &str[start], length);

  substr[length] = '\0';

  return substr;
}

string string_substring_range(const_string str, int start, int end)
{
  ASSERT(start < end);

  return string_substring(str, start, end - start);
}

size_t string_length(const_string str)
{
  return strlen(str);
}