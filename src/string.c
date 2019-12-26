#include <stddef.h>

size_t strlen(char *string)
{
  char c = string[0];
  size_t len = 0;

  while (c != '\0')
  {
    len++;
    c = string[len];
  }

  return len;
}

i32 strcmp(char *str1, char *str2)
{
    i32 i=0;
    while(str1[i] != '\0' && str2[i] != '\0' && (str1[i] == str2[i])) 
    {
        i++;
    }
    if(str1[i] == '\0' && str2[i] == '\0') 
    {
        return 0;
    }
    if(str1[i] == '\0' && str2[i] != '\0') 
    {
        return -1;
    }
    if(str1[i] != '\0' && str2[i] == '\0') 
    {
        return 1;
    }
}

i32 strncmp(char *str1, char *str2, i32 n)
{
    i32 i=0;
    while(str1[i] != '\0' && str2[i] != '\0' && (str1[i] == str2[i]) && i < n) 
    {
        i++;
    }
    if(i==n) 
    {
        return 0;
    }
	return 1;
}
