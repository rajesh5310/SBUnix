#include <defs.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <print.h>
#include <sys/timer.h>
void start(void* modulep, void* physbase, void* physfree)
{
    print_line(50, 0, "Welcome to OS");

    print("Character : %c\n", 'a');
    print("String : %s\n", "Rajesh");
    print("Integer : %d\n", 506 );
    print("Hex Code : 0x%x\n",  10324);
    int var = 10;
    print("Pointer : %p\n", &var);
    init_timer();
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
	clear_screen();
	reload_gdt();
    reload_idt();
    load_irq();

    /*int infinite_loop=1;
    while(infinite_loop);*/
	start(
		(char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase,
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);

	while(1);}
