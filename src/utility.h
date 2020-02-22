
#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <stddef.h>

void *set_memory(void *address, char data, size_t num_bytes);
void *memset(void *address, char data, size_t num_bytes);
void port_out(uint16_t port, uint8_t byte);
uint8_t port_in(uint16_t port);
void io_wait(void);

#endif
