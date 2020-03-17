
#include <stddef.h>
#include <stdint.h>
#include <interrupts.h>
#include <scheduler.h>
#include <utility.h>
#include <display.h>
#include <pit.h>

static void timer();
extern uint32_t scheduling;
uint8_t ticks;
uint8_t seconds = 0;
uint16_t hz = 0;

// Setting a default frequency of 100hz. Can be altered later on...
void install_timer()
{
  reg_int_handler(IRQ0, &timer);
  set_frequency(100);
}

static void timer(struct reg_state r)
{
  ticks++;
  if(ticks % TIME_SLICE == 0){
    if(scheduling == 1)
      switch_tasks(r);
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

  hz = freq;
  uint32_t divisor = TICK_RATE / freq;
  uint8_t div_low = (uint8_t)(divisor & 0xFF);
  uint8_t div_high = (uint8_t)((divisor >> 8) & 0xFF);

  port_out(COMMAND_PORT, COMMAND_BYTE);
  port_out(CHANNEL0, div_low);
  port_out(CHANNEL0, div_high);
}

uint8_t get_time(){ return ticks; }

void timer_wait(int tick_amount)
{
    uint32_t t_end = ticks + tick_amount * hz;
    // event queue on timer interrupt

    // deadlock from there
    while(seconds < t_end);
}
