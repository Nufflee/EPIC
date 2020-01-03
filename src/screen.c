#include <stddef.h>
#include "screen.h"
#include "io.h"
#include "serial_port.h"

void screen_init()
{
  screen_clear();

  // Enable cursor
  out8(0x3D4, 0x0A);
  out8(0x3D5, (in8(0x3D5) & 0xC0) | 0);
  out8(0x3D4, 0x0B);
  out8(0x3D5, (in8(0x3D5) & 0xE0) | 0);

  screen_set_cursor_position(0, 0);
}

void screen_clear()
{
  for (u16 x = 0; x < VGA_WIDTH; x++)
    for (u16 y = 0; y < VGA_HEIGHT; y++)
    {
    //needs to be volatile dew to the memory address
     volatile u16 * VGA_Display_Address_And_ScreenLocation = (volatile u16 *)0xB8000 + (y * VGA_WIDTH + x) ;
     *VGA_Display_Address_And_ScreenLocation = '\0' | (0 << 8);
    }
}

void screen_set_cursor_position(i8 x, i8 y)
{
  u16 index = (u16)(y * VGA_WIDTH + x);

  out8(0x3D4, 0x0F);
  out8(0x3D5, (u8)(index & 0xFF));
  out8(0x3D4, 0x0E);
  out8(0x3D5, (u8)((index >> 8) & 0xFF));
}

void screen_draw_char_at(u8 x, u8 y,u8 forecolour, u8 backcolour,u8 TextLightMode, char c)
{
  u16 Screen_data_to_be_drawn;

  if(TextLightMode == 1)
  {
    Screen_data_to_be_drawn = (backcolour << 4) | (forecolour & 0x0F);
  }
  else
  {
    Screen_data_to_be_drawn = (backcolour << 4) | ((forecolour + 8) & 0x0F);
  } 
  //needs to be volatile dew to the memory address
  volatile u16 * VGA_Display_Address_And_ScreenLocation = (volatile u16 *)0xB8000 + (y * VGA_WIDTH + x) ;
  *VGA_Display_Address_And_ScreenLocation = c | (Screen_data_to_be_drawn << 8);
}