
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define VGA_ADDRESS 0xB8000
#define BUF_SIZE 2000
#define ASCII_ZERO 48
#define VGA_COM_PORT 0x3D4
#define CURSOR_HIGH_BYTE 14
#define CURSOR_LOW_BYTE 15

static const size_t TERMINAL_WIDTH = 80;
static const size_t TERMINAL_HEIGHT = 25;

enum vga_color {
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHT_GREY,
	DARK_GREY,
	LIGHT_BLUE,
	LIGHT_GREEN,
	LIGHT_CYAN,
	LIGHT_RED,
	LIGHT_MAGENTA,
	LIGHT_BROWN,
	WHITE,
};

void print(const char* string, uint8_t fcol, uint8_t bcol);
uint16_t vga_input(unsigned char chr, uint8_t fcolour, uint8_t bcolour);
void move_entry(size_t x,  size_t y);
void clear_terminal();
void move_cursor(int x, int y);
int get_cursor_y();
int get_cursor_x();
void set_cursor_x(int x);
void scroll_terminal();
void putchar(char chr, uint8_t fcol, uint8_t bcol);
void println(char* fstring, ...);
char *base_conversion(unsigned int decimal, int base);
void print_error(char* string);

#endif
