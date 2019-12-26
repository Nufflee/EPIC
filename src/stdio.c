#include <string.h>
#include "terminal.h"

size_t printf(char *message)
{
  size_t len = strlen(message);
/*
 * for ever letter in the message varable it will write to 
 *  the display memory buffer to desplay tto the screen
 */
  for (size_t i = 0; i < len; i++) terminal_putchar(message[i]);
  //after message is displayed on screen then it returns the original message car size
  return len;
}