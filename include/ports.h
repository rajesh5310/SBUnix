/* @name : ports.h
 * @author : rgolani, abmishra, skandalamsri
 * @last updated date : 25th September, 2013
 */

#ifndef PORTS_H
#define PORTS_H

#include "defs.h"

inline void outb(uint16_t port, uint8_t data);
void memset(void *dest, uint8_t val, uint32_t len);
inline uint8_t port_inb(uint16_t port);
#endif
