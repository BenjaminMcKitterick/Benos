
#include "../interrupts/interrupts.h"
#include "../utility/utility.h"
#include "../utility/display.h"
#include "keyboard.h"

static void keyboard_input();
void key_enter();
int entry_x = ENTRY_START;
char string_input[128] = "";
uint8_t string_length = 0;
int entry_y;
uint8_t first_key = 0;

unsigned char keymap[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

void install_keyboard()
{
  reg_int_handler(IRQ1, &keyboard_input);
}

void update_keyboard(){
  entry_x = get_cursor_x();;
  entry_y = get_cursor_y()+1;
  move_entry(entry_x, entry_y);
  println("Ben%cs ->", (char)1);
  entry_x = ENTRY_START;
  move_entry(entry_x, entry_y);
}

static void keyboard_input()
{
    if(first_key == 0)
    {
        first_key = 1;
    }
    unsigned char scancode = port_in(0x60);
    char clear = (0x20 | 0 << 8);
    move_entry(entry_x, entry_y);
    char key = keymap[scancode];

    if(scancode & 0x80){
        // handle shift keys and tings...
    } else {
      string_input[string_length] += key;
      string_length++;
      switch(key){
        case '\n':
          check_command();
          break;
        case '\b':
          if(entry_x > ENTRY_START){
            move_entry(entry_x-1, entry_y);
            putchar(clear, LIGHT_BROWN, BLACK);
            entry_x--;
          }
          break;
        default:
          putchar(keymap[scancode], LIGHT_BROWN, BLACK);
          entry_x++;
          break;
      }
    }
}

void key_enter(){
    if(entry_x > (int)(TERMINAL_WIDTH-ENTRY_START)){
      int line = (int)(entry_x/TERMINAL_WIDTH);
      entry_y += line + 1;
    }else {
      entry_y++;
    }
    if (get_cursor_y() >= (int)(TERMINAL_HEIGHT-1)){
        scroll_terminal();
        entry_y = TERMINAL_HEIGHT-1;
    }
    entry_x = 0;
    move_entry(entry_x, entry_y);
    println("Ben%cs ->", (char)1);
    entry_x = ENTRY_START;
}

void check_command(){
  string_input[string_length-1] = '\0';
  string_length = 0;
  char* str = "clear";
  if(*string_input == *str){
      reset_terminal();
      memset(string_input, 0, sizeof(string_input));
  }
  else
  {
      key_enter();
  }
}

void set_entry(int x, int y)
{
  entry_x = x;
  entry_y = y;
}
