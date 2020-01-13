
#include <stdint.h>
#include <stdarg.h>
#include "display.h"
#include "utility.h"
#include "keyboard.h"

static size_t vga_index;
static uint16_t* vga_buf;
int cursor_x;
int cursor_y;

/*
   Start of the vga buffer is defined then a while loop is used to iterate through the
   characters in the string to print them to the terminal.
*/

void print(const char *string, uint8_t fcol, uint8_t bcol)
{
	vga_buf = (uint16_t*) VGA_ADDRESS;
	while( *string != 0 )
	{
	    vga_buf[vga_index] = vga_input(*string++, fcol, bcol);
	    vga_index++;
	}
}

void putchar(char chr, uint8_t fcol, uint8_t bcol){
    vga_buf = (uint16_t*) VGA_ADDRESS;
    vga_buf[vga_index] = vga_input(chr, fcol, bcol);
    vga_index++;
}

/*
   Initially bitwise OR is used for the background and foreground colour to set the bits.
   Then bitwise OR is used once more the combine the colour and the character for the vga
   buffer.
*/

uint16_t vga_input(unsigned char chr, uint8_t fcolour, uint8_t bcolour ){
	uint8_t colour = (uint8_t) fcolour | (uint8_t) bcolour << 4;
	return (uint16_t) chr | (uint8_t) colour << 8;
}

/*
  Changes the vga_index for vga_buf so that strings can be displayed in any part
  of the terminal. Moves the cursor to the place of entry.
*/

void move_entry(size_t x,  size_t y)
{
	vga_index = y * TERMINAL_WIDTH + x;
	uint16_t cursor_pos = vga_index;
	port_out(0x3D4, CURSOR_HIGH_BYTE);
	port_out(0x3D5, cursor_pos >> 8);
	port_out(0x3D4, CURSOR_LOW_BYTE);
	port_out(0x3D5, cursor_pos);

	cursor_x = x;
	cursor_y = y;
}

int get_cursor_x(){	return cursor_x; }
int get_cursor_y(){ return cursor_y; }
void set_cursor_x(int x){ cursor_x = x; }

void clear_terminal()
{
    for(int i = 0; i < (int)(TERMINAL_WIDTH*TERMINAL_HEIGHT); i++){
        vga_buf[i] = 0x20 | 0 << 8;
    }
}

void scroll_terminal()
{
		// move the text chunk up
		for(int i = 0*TERMINAL_WIDTH; i < (int)((TERMINAL_HEIGHT-1)*TERMINAL_WIDTH); i++){
				vga_buf[i] = vga_buf[i+TERMINAL_WIDTH];
		}
		// clear bottom line
		for(int i = (TERMINAL_HEIGHT-1)*TERMINAL_WIDTH; i < (int)(TERMINAL_HEIGHT*TERMINAL_WIDTH); i++){
				vga_buf[i] = 0x20 | 0 << 8;
		}
		cursor_y = TERMINAL_HEIGHT-1;
}

void println(char* fstring, ...)
{
  va_list arg;
  va_start(arg, fstring);
  unsigned int i;
  char *str;

  while(*fstring != 0)
  {
      if(*fstring == '%'){
          fstring++;
          switch(*fstring)
          {
              case 'd':
                  i = va_arg(arg, int);
                  //putchar((char)(ASCII_ZERO+i), GREEN, BLACK);
									print(base_conversion(i, 10), GREEN, BLACK);
                  break;
              case 'c':
                  i = va_arg(arg, int);
                  putchar(i, GREEN, BLACK);
                  break;
              case 's':
                  str = va_arg(arg, char*);
                  print(str, GREEN, BLACK);
                  break;
              case 'h':
                  i = va_arg(arg, unsigned int);
									print("0x", BLUE, BLACK);
                  print(base_conversion(i, 16), GREEN, BLACK);
                  break;
          }
      } else {
          putchar(*fstring, GREEN, BLACK);
      }
      fstring++;
  }
  va_end(arg);
	move_entry(get_cursor_x(), get_cursor_y()+1);
}

char *base_conversion(unsigned int decimal, int base)
{
  static char hex_val[50];
  char *pointer;

  // set end value to \0
  pointer = &hex_val[49];
  *pointer = '\0';

  while(decimal != 0)
  {
      int remainder = 0;
      remainder = decimal%base;

      if(remainder < 10)
      {
          *--pointer = 48 + remainder;
      } else {
          *--pointer = 55 + remainder;
      }
      decimal = decimal/base;
  }
  return pointer;
}
