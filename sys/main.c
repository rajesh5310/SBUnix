#include <defs.h>
#include <sys/gdt.h>
#include <print.h>
#include <sys/vmem.h>
#include <sys/idt.h>
#include <sys/timer.h>
#include <sys/tarfs.h>
#include <process.h>
#include <fs.h>
void start(uint32_t* modulep, void* physbase, void* physfree)
{
	 	mm_init(modulep,physbase,physfree);
	    reload_gdt();
		reload_idt();
		load_irq();

		init_timer();
		setup_tss();
		first_cs();
	    print("testing 123");
	    print("\n%d", 123);
	    print("\n%s", "Rajesh");
	    print("\n%c", 'q');
	    int i = 0;
	    print("\n0x%x", &i);
        // probe_port((HBA_MEM *)(KERNBASE+(uintptr_t)0xfebf1000)); 
		// kernel starts here
		while(1);
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
//	register char *temp1, *temp2;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);

	/*To clear the screen*/
	clear_screen();


	//setup_tss();
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	/*for(
		temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
		*temp1;
		temp1 += 1, temp2 += 2
	) *temp2 = *temp1;*/
	while(1);
}
