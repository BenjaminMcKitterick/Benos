
#ifndef PIT_H
#define PIT_H

#define COMMAND_PORT 0x43
#define COMMAND_BYTE 0x36
#define CHANNEL0 0x40
#define TICK_RATE 1193180

void install_timer();
void set_frequency(uint8_t freq);
void timer_wait(int tick_amount);
static void timer();

#endif
