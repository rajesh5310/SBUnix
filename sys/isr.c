/* @name : isr.c
 * @author : rgolani, abmishra, skandalamsri
 * @last updated date : 25th September, 2013
 */

#include<defs.h>
#include<print.h>
#include<ports.h>
#include<keyboard.h>
#include <sys/timer.h>
#include<sys/isr.h>

/*
* Structure that returns parameters from the ISR routines
*/
struct isr_stack_frame
{
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

/*
* Divide-by-zero ISR handler
*/
void isr_handler0(struct isr_stack_frame *stack) {

        print("DIVIDE BY ZERO ERROR!\n");
        print("    CS:0x%x\n",stack->cs);
        print("   RIP:0x%x\n",stack->rip);
      __asm__( "hlt" );
}

/*
* Mapped to IRQ-0. Hardware timer handler.
*/
void isr_handler32(struct isr_stack_frame *stack) {

        outb(0x20, 0x20);
        timer_callback();
}

/*
* Mapped to IRQ-1. Keyboard interrupt habdler.
*/
void isr_handler33(struct isr_stack_frame *stack) {

        outb(0x20, 0x20);
        handle_keyboard_irq();
}
