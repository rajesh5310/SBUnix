/*
 * =====================================================================================
 *
 *       Filename:  process.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/12/2013 04:02:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rajesh Golani
 *        Company:  Stony Brook University
 *
 * =====================================================================================
 */
#include <process.h>
#include <defs.h>
#include <print.h>
#include <sys/pmap.h>
#include <sys/gdt.h>
#include <sys/tarfs.h>
#include <utils.h>
#include <string.h>
static void load_icode(task *t, char *filename, size_t size);
#define yield() asm("int $0x80")

extern physaddr_t boot_cr3;
void * get_kva(struct Page *pp) {
	physaddr_t ppn_addr = page2ppn(pp) << PGSHIFT;
	//uint32_t __m_ppn = ppn_addr >> PTXSHIFT ;
	uint64_t kva_addr = ppn_addr + KERNBASE;
	return (void *) kva_addr;
}

extern pml4e_t *boot_pml4e;

task thread1;
task thread2;

int num_process = 2;
task readyQ[5];

bool flag = true;

void function1();
void function2();

static __inline void lcr3(uint64_t val) {
	__asm __volatile("movq %0,%%cr3" : : "r" (val));
}

void first_cs() {

	struct Page *pp1 = NULL;
	struct Page *pp2 = NULL;

	//	int i = 0;
	// initialize both the task structures
	// set the address of function1 to the start of the stack of thread1 and same for thread2
	// set the rsp to point to the stack

	//int i = 0;
	//  thread2.stack[i] = 0;
	//}

	pp1 = page_alloc(0);
	pp2 = page_alloc(0);
	//  print("\n%x", page2pa(pp1));
	uint64_t *pml4a = (uint64_t *) get_kva(pp1);
	uint64_t *pml4b = (uint64_t *) get_kva(pp2);
	// uint64_t *pml4a=(uint64_t *)page2kva(pp1);
	// uint64_t *pml4b=(uint64_t *)page2kva(pp2);

	pml4a[511] = boot_pml4e[511]; //point to pdpe of kerne
	pml4b[511] = boot_pml4e[511]; //point to pdpe of kerne

	thread1.cr3 = PADDR(pml4a);
	thread1.pml4e = pml4a;
	thread2.cr3 = PADDR(pml4b);
	thread2.pml4e = pml4b;


	load_icode(&thread1, "bin/hello", 0);

	print("\n thread.entry %x", thread1.entry);
	thread1.stack[59] = (uint64_t) thread1.entry;
	//thread1.stack[59] = (uint64_t) &function1;
	thread1.rsp = (uint64_t) &thread1.stack[59];

	thread1.stack[63] = 0x23; //  Data Segment
	thread1.stack[62] = (uint64_t) &thread1.stack[63]; //  RIP
	//thread1.stack[61] = 0x20202 ;                           //  RFlags
	thread1.stack[61] = 0x246; //  EFlags
	thread1.stack[60] = 0x1b; // Code Segment*/

	/*load_icode(&thread2, "bin/hello1", 0);
	print("\n thread.entry %x", thread2.entry);

	thread2.stack[59] = (uint64_t) thread2.entry;
	//thread2.stack[59] = (uint64_t) &function2;
	thread2.rsp = (uint64_t) &thread2.stack[59];

	thread2.stack[63] = 0x23; //  Data Segment
	thread2.stack[62] = (uint64_t) &thread2.stack[63]; //  RIP
	//thread1.stack[61] = 0x20202 ;                           //  RFlags
	thread2.stack[61] = 0x246; //  EFlags
	thread2.stack[60] = 0x1b; // Code Segment*/

	// inilialize the ready queue with both the task structures
	readyQ[0] = thread1;
	readyQ[1] = thread2;

	// load the value of rsp of thread1 into the kernel rsp
	// this will cause context switch


	__asm__("cli");
	asm volatile("movq %0, %%cr3":: "a"(thread1.cr3));
	__asm__("sti");

	print("\nI am in process virtual address space");

	//  __asm__ __volatile__("cli");
	__asm__ __volatile__ (
			"movq %0, %%rsp;" //load next's stack in rsp
			:
			:"r"(thread1.rsp)
	);

	/*__asm__ __volatile__( "popq %r11");
	__asm__ __volatile__( "popq %r10");
	__asm__ __volatile__( "popq %r9");
	__asm__ __volatile__( "popq %r8");
	__asm__ __volatile__( "popq %rdi");
	__asm__ __volatile__( "popq %rsi");
	__asm__ __volatile__( "popq %rdx");
	__asm__ __volatile__( "popq %rcx");
	__asm__ __volatile__( "popq %rax");*/
	//  print("Going back to kernel space\n");

	//  asm volatile( "cli");

	//   asm volatile("movq %0, %%cr3":: "b"(kernel_pml4e));
	//   asm volatile( "sti");

	//   print("Back to kernel space\n");

	/*__asm__ __volatile__(
				"popq %rdx;"
				"popq %rcx;"
				"popq %rbx;"
				"popq %rax;"
		);*/

/*    __asm__ __volatile__(
        "movq %0, %%r15;"
        :
        :"r"(&thread1.stack[63])
    );

    __asm__ __volatile__(
        "movq %%r15, %0;"
        :"=r"(tss.rsp0)
    );*/

	print("\nbefore tss\n");

    /*asm volatile("mov $0x2b,%ax");
    asm volatile("ltr %ax");
	tss.rsp0 = (uint64_t)&thread1.stack[63];
	*/__asm__ __volatile__("iretq");
}

void switch_to(task* prev, task* next) {
	// will save the content prev task on the stack
	// will update the value of the current rsp to the point to the rsp of the next task
	// this will cause the context switch from prev task to next task
	// TODO: write the assembly language code
	/*__asm__ __volatile__ (
			"pushq %rax;"
			"pushq %rbx;"
			"pushq %rcx;"
			"pushq %rdx;"
	);*/
	__asm__ __volatile__(
			"movq %%rsp, %0;"
			:"=m"(prev->rsp)
			:
			:"memory"
	);
	asm volatile("movq %0, %%cr3":: "a"(next->cr3));
	__asm__ __volatile__ (
			"movq %0, %%rsp;"
			:
			:"m"(next->rsp)
			:"memory"
	);
	/*__asm__ __volatile__(
			"popq %rdx;"
			"popq %rcx;"
			"popq %rbx;"
			"popq %rax;"
	);*/

	 __asm__ __volatile__(
	        "movq %0, %%r15;"
	        :
	        :"r"(next->stack[63])
	    );

	    /*__asm__ __volatile__(
	        "movq %%r15, %0;"
	        :"=r"(tss.rsp0)
	    );*/

	    /*asm volatile("mov $0x2b,%ax");
	    asm volatile("ltr %ax");*/

	 tss.rsp0 = (uint64_t)next->stack[63];
	__asm__("iretq");
}

void schedule() {
	// will halt the currently executing thread
	// will pop the next task from the ready queue
	// call switch_to function with prev and next task

	//switch_to(&readyQ[1], &readyQ[0]);


	if (flag) {
		flag = false;
		switch_to(&readyQ[0], &readyQ[1]);
	} else {
		flag = true;
		switch_to(&readyQ[1], &readyQ[0]);
	}


}

void function1() {
		print("\nHello\n");
		while(1);
		//schedule();
		//yield();
}

void function2() {
		print("World..!!");
		while(1);
		//schedule();
		//yield();
}

static void region_alloc(task *t, void *va, size_t len) {
	void *va_start = ROUNDDOWN(va, PGSIZE);
	void *va_end = ROUNDUP((char*)va+len, PGSIZE);
	struct Page *p;
	char *v;
	for (v = (char*) va_start; v < (char*) va_end; v += PGSIZE) {
		if (!(p = page_alloc(ALLOC_ZERO)))
			panic("Running out of memory\n");
		else {
			if (page_insert((pml4e_t *) t->pml4e, p, v, PTE_P | PTE_U | PTE_W)
					!= 0)
				panic("Running out of memory\n");
		}
	}
}

// Set up the initial program binary, stack, and processor flags
// for a user process.
// This function is ONLY called during kernel initialization,
// before running the first user-mode environment.
//
// This function loads all loadable segments from the ELF binary image
// into the environment's user memory, starting at the appropriate
// virtual addresses indicated in the ELF program header.
// At the same time it clears to zero any portions of these segments
// that are marked in the program header as being mapped
// but not actually present in the ELF file - i.e., the program's bss section.
//
// All this is very similar to what our boot loader does, except the boot
// loader also needs to read the code from disk. Take a look at
// boot/main.c to get ideas.
//
// Finally, this function maps one page for the program's initial stack.
//
// load_icode panics if it encounters problems.
// - How might load_icode fail? What might be wrong with the given input?
//
static void load_icode(task *t, char *filename, size_t size) {
	// Hints:
	// Load each program segment into virtual memory
	// at the address specified in the ELF section header.
	// You should only load segments with ph->p_type == ELF_PROG_LOAD.
	// Each segment's virtual address can be found in ph->p_va
	// and its size in memory can be found in ph->p_memsz.
	// The ph->p_filesz bytes from the ELF binary, starting at
	// 'binary + ph->p_offset', should be copied to virtual address
	// ph->p_va. Any remaining memory bytes should be cleared to zero.
	// (The ELF header should have ph->p_filesz <= ph->p_memsz.)
	// Use functions from the previous lab to allocate and map pages.
	//
	// All page protection bits should be user read/write for now.
	// ELF segments are not necessarily page-aligned, but you can
	// assume for this function that no two segments will touch
	// the same virtual page.
	//
	// You may find a function like region_alloc useful.
	//
	// Loading the segments is much simpler if you can move data
	// directly into the virtual addresses stored in the ELF binary.
	// So which page directory should be in force during
	// this function?
	//
	// You must also do something with the program's entry point,
	// to make sure that the environment starts executing there.
	// What? (See env_run() and env_pop_tf() below.)

	// LAB 3: Your code here.

	uint64_t offset = is_file_exists(filename);

	if (offset == 0 || offset == 999) {
		print("\n Error. File not found in tarfs.");
		offset = 0;
	} else {
		Elf_hdr *elf = (Elf_hdr *) (&_binary_tarfs_start + offset);

		Elf64_Phdr *ph, *eph;
		// struct Page *p = NULL;
		ph = (Elf64_Phdr *) ((uint8_t *) elf + elf->e_phoff);

		eph = ph + elf->e_phnum;

		for (; ph < eph; ph++) {
			if (ph->p_type == ELF_PROG_LOAD) {
				//int64_t va = ph->p_vaddr;
				// uint64_t size = ph->p_memsz;
				//uint64_t offset = ph->p_offset;
				//uint64_t i = 0;
				if (ph->p_filesz > ph->p_memsz)
					print("Wrong size in elf binary\n");
				print("\n vaddr : %x size %x", ph->p_vaddr, ph->p_memsz);

				region_alloc(t, (void*) ph->p_vaddr, ph->p_memsz);
				// Switch to env's address space so that we can use memcpy
				lcr3(t->cr3);
				memcpy((char*) ph->p_vaddr, (void *) elf + ph->p_offset,
						ph->p_filesz);

				if (ph->p_filesz < ph->p_memsz)
					memset((char*) ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz
							- ph->p_filesz);

				// Switch back to kernel's address space
				lcr3(boot_cr3);
			}
		}

		// Now map one page for the program's initial stack
		// at virtual address USTACKTOP - PGSIZE.
		//region_alloc(e, (void*) (USTACKTOP - PGSIZE), PGSIZE);

		// Magic to start executing environment at its address.
		t->entry = elf->e_entry;
	}

}
