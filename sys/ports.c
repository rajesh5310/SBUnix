#include<defs.h>

inline void outb(uint16_t port, uint8_t data) {

    __asm__ __volatile( "outb %0, %1;"
            : /* void */
            : "a" (data), "d" (port));

}

void memset(void *dest, uint8_t val, uint32_t len)
{
    uint8_t *temp = (uint8_t *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

inline uint8_t port_inb(uint16_t port) {

    uint8_t ret;
    __asm__ __volatile__( "inb  %1, %0;"
            :"=a" (ret)
            :"d"  (port)   );
    return ret;
}
