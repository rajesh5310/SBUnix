#include <defs.h>
#include <sys/gdt.h>
#include <print.h>
#include <sys/vmem.h>
#include <sys/idt.h>
#include <sys/timer.h>
void start(uint32_t* modulep, void* physbase, void* physfree)
{
	print("Integer : %d\n", 506 );
    /*print("\n Physbase %x", physbase);
    print("\n Physfree %x", physfree);
    char *physfree_c = (char *)physfree;
    init_phy_mem(modulep, physbase, physfree_c);*/
    //mm_init(modulep,physbase,physfree);

    /*To load GDT*/
	reload_gdt();
	/*To load IDT*/
	reload_idt();
	/*int i = 0;
	print("%d", 1/i);*/
	/*To load PIC interrupts*/
	load_irq();
   print_line(50, 0, "Welcome to SBUnix");

   print("Character : %c\n", 'a');
   print("String : %s\n", "Rajesh");
   print("Integer : %d\n", 506 );
   print("Hex Code : 0x%x\n",  10324);
   int var = 10;
   print("Pointer : 0x%p\n", &var);

   //To intialize timer
   init_timer();
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


		setup_tss();
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
