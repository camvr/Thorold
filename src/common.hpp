#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

#endif
