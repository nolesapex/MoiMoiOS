#ifndef IO_H
#define IO_H

#include "types.h"

// Basic I/O functions (e.g., for serial port or screen output)
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

#endif // IO_H

