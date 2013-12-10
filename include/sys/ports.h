#ifndef PORTS_H
#define PORTS_H

#include "defs.h"
#include "string.h"

 void outb(uint16_t port, uint8_t data);
inline uint8_t port_inb(uint16_t port);
#endif
