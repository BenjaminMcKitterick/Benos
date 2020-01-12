
#include "interrupts.h"
#include "utility.h"
#include "display.h"
#include "pit.h"

#include <stddef.h>
#include <stdint.h>

uint8_t ticks;
uint8_t seconds;

// Setting a default frequency of 100hz. Can be altered later on...
void install_timer()
{
  reg_int_handler(IRQ0, &timer);
  set_frequency(100);
}

static void timer()
{
  ticks++;
  if(ticks % 100 == 0){
    seconds++;
  }
}
/*
   The divisor is calculated and then split into the MSB
   and LSB. We then send the command byte to the command
   register. This tells the timer that we dont want to use
   BCD and that we want square wave mode.
*/
void set_frequency(uint8_t freq){

  uint32_t divisor = TICK_RATE / freq;
  uint8_t div_low = (uint8_t)(divisor & 0xFF);
  uint8_t div_high = (uint8_t)((divisor >> 8) & 0xFF);

  port_out(COMMAND_PORT, COMMAND_BYTE);
  port_out(CHANNEL0, div_low);
  port_out(CHANNEL0, div_high);
}

void timer_wait(int tick_amount)
{
    //uint32_t t_end = ticks + t;
    unsigned long eticks;

    eticks = ticks + tick_amount;
    //println("%d",eticks);
    while(ticks < eticks){
      //println(" %d %d", seconds, eticks);
    }
    return 0;
}
