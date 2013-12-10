/* kbc.h
 *
 *  Created on: 9 Aug 2011
 *      Author: Chris Stones
 */

#ifndef X86_64_ARCH_KBC_H_
#define X86_64_ARCH_KBC_H_

#include <defs.h>

void			kbc_irq();

void 			kbc_initialise();
size_t			kbc_bytes_in_buffer();
sint32_t		kbc_readchar();
sint32_t		kbc_read(void* dst, size_t size);
void            scanf(char *);
/*Scan codes*/
extern char keyboard_buf[100];
extern volatile int is_taking_input;

uint8_t get_tochar(uint8_t b, uint8_t shift , uint8_t ctrl);
#endif /* X86_64_ARCH_KBC_H_ */
