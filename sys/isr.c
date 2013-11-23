/* @name : isr.c
 * @author : rgolani, abmishra, skandalamsri
 * @last updated date : 25th September, 2013
 */

#include<defs.h>
#include<print.h>
#include<ports.h>
#include<keyboard.h>
#include<sys/timer.h>
#include<sys/isr.h>
#include<process.h>
/*
 * Structure that returns parameters from the ISR routines
 */
struct isr_stack_frame {
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
	print("    CS:0x%x\n", stack->cs);
	print("   RIP:0x%x\n", stack->rip);
	__asm__( "hlt" );
}
void isr_handler1(struct isr_stack_frame *stack) {

	print("isr 1\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler2(struct isr_stack_frame *stack) {

	print("isr 2\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler3(struct isr_stack_frame *stack) {

	print("isr 3\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler4(struct isr_stack_frame *stack) {

	print("isr 4\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler5(struct isr_stack_frame *stack) {

	print("isr 5\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler6(struct isr_stack_frame *stack) {

	print("isr 6\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler7(struct isr_stack_frame *stack) {

	print("isr 7\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler8(struct isr_stack_frame *stack) {

	print("isr 8\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler9(struct isr_stack_frame *stack) {

	print("isr 9\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler10(struct isr_stack_frame *stack) {

	print("isr 10\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler11(struct isr_stack_frame *stack) {

	print("isr 11\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler12(struct isr_stack_frame *stack) {

	print("isr 12\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler13(struct isr_stack_frame *stack) {

	print("isr 13\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}
void isr_handler14(struct isr_stack_frame *stack) {

	print("isr 14\n");
	print("    CS:0x%x\n", stack->cs);
	print("   RIP:0x%x\n", stack->rip);
	__asm__( "hlt" );
}

void isr_handler15(struct isr_stack_frame *stack) {

	print("isr 15\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler16(struct isr_stack_frame *stack) {

	print("isr 16\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler17(struct isr_stack_frame *stack) {

	print("isr 17\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler18(struct isr_stack_frame *stack) {

	print("isr 18\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler19(struct isr_stack_frame *stack) {

	print("isr 19\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler20(struct isr_stack_frame *stack) {

	print("isr 20\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler21(struct isr_stack_frame *stack) {

	print("isr 21\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler22(struct isr_stack_frame *stack) {

	print("isr 22\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler23(struct isr_stack_frame *stack) {

	print("isr 23\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler24(struct isr_stack_frame *stack) {

	print("isr 24\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler25(struct isr_stack_frame *stack) {

	print("isr 25\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler26(struct isr_stack_frame *stack) {

	print("isr 26\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler27(struct isr_stack_frame *stack) {

	print("isr 27\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler28(struct isr_stack_frame *stack) {

	print("isr 28\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler29(struct isr_stack_frame *stack) {

	print("isr 29\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler30(struct isr_stack_frame *stack) {

	print("isr 30\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
	__asm__( "hlt" );
}

void isr_handler31(struct isr_stack_frame *stack) {

	print("isr 31\n");
	//        print("    CS:0x%x\n",stack->cs);
	//        print("   RIP:0x%x\n",stack->rip);
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
void isr_handler128(struct isr_stack_frame *stack) {
	/*int d=current_task->pid;
	 printf("Value %d\n",d);
	 */
	uint64_t syscall_no = 0, ch = 0;

	__asm__ __volatile__(
			"movq %%rax, %0;"
			:"=a"(syscall_no)
	);

	if(syscall_no == 2)
	{
			__asm__ __volatile__(
					"movq %%rbx, %0;"
					:"=b"(ch)
			);

			//print("\n%d", syscall_no);
			print((char *)&ch);

	}

	//schedule();
	/*if(stack->rax == 1){
	 //call the exit system call
	 __exit();
	 }
	 else if(s->rax == 2){
	 //call for printf
	 printf("Value %d\n",s->rbx);
	 __putchar(s->rbx);
	 }

	 else if(s->rax == 3){
	 //call for maybe fork
	 }
	 */
}
