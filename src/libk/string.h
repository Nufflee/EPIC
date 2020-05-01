#pragma once

#include <stddef.h>

typedef char *string;
typedef const char *const_string;

string string_concat(string destination, const_string source);
int string_compare(const_string str1, const_string str2);
string *string_split(const_string str, const_string delimiter);
string string_normalize_spaces(const_string str);
int string_find(const_string str, const_string needle);
string string_substring(const_string str, int start, int length);
string string_substring_range(const_string str, int start, int end);
size_t string_length(const_string str);