/*
 * =====================================================================================
 *
 *       Filename:  process.c
 *
 *    Description:  i
 *
 *        Version:  1.0
 *        Created:  11/16/2013 03:17:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Er. Udit Gupta (ukg), udit.gupta@stonybrook.edu
 *        Company:  Stony Brook University
 *
 * =====================================================================================
 */

#include <process.h>
#include <defs.h>
#include <print.h>
#include <sys/pmap.h>
#include <sys/gdt.h>
#include <sys/timer.h>

void * get_kva(struct Page *pp)
{
    physaddr_t ppn_addr= page2ppn(pp) << PGSHIFT;
    //uint32_t __m_ppn = ppn_addr >> PTXSHIFT ;
    uint64_t kva_addr = ppn_addr + KERNBASE; 
    return (void *)kva_addr;
}

extern pml4e_t *boot_pml4e;
/*
task thread1;
task thread2;

int num_process = 2;


void function1();
void function2();
*/
void first_cs() {
    
     struct Page   *pp1=NULL;
     struct Page   *pp2=NULL;

     int i=0;
    // initialize both the task structures
    // set the address of function1 to the start of the stack of thread1 and same for thread2
    // set the rsp to point to the stack

    //int i = 0;
    //  thread2.stack[i] = 0;
    //}

    pp1=page_alloc(0);
    pp2=page_alloc(0);
   
  //  print("\n%x", page2pa(pp1)); 
    uint64_t *pml4a=(uint64_t *)get_kva(pp1);
    uint64_t *pml4b=(uint64_t *)get_kva(pp2);
   // uint64_t *pml4a=(uint64_t *)page2kva(pp1);
   // uint64_t *pml4b=(uint64_t *)page2kva(pp2);


    for(i=0;i<512;i++) {
        pml4a[i] = 0;
        pml4b[i] = 0;
    }

    pml4a[511] = boot_pml4e[511]; //point to pdpe of kerne
    pml4b[511] = boot_pml4e[511]; //point to pdpe of kerne

    thread1.cr3 = PADDR(pml4a);
    thread2.cr3 = PADDR(pml4b);

    thread1.stack[59] = (uint64_t)&function1;   
    thread1.rsp = (uint64_t)&thread1.stack[49];

    thread1.stack[63] = 0x23 ;                              //  Data Segment    
  //  thread1.stack[62] = (uint64_t)USTACK;      //  RIP
    thread1.stack[62] = (uint64_t)&thread1.stack[63] ;      //  RIP
    //thread1.stack[61] = 0x20202 ;                           //  RFlags
    thread1.stack[61] = 0x246;                           //  EFlags
    thread1.stack[60] = 0x1b ;                              // Code Segment
 

    thread2.stack[63] = 0x23 ;                              //  Data Segment    
  //  thread2.stack[62] = (uint64_t)USTACK;      //  RIP
   thread2.stack[62] = (uint64_t)&thread2.stack[63] ;      //  RIP
    //thread1.stack[61] = 0x20202 ;                           //  RFlags
    thread2.stack[61] = 0x246;                           //  EFlags
    thread2.stack[60] = 0x1b ;                              // Code Segment



    thread2.stack[59] = (uint64_t)&function2;   
    thread2.rsp = (uint64_t)&thread2.stack[49];     
    
    // inilialize the ready queue with both the task structures
    readyQ[0] = thread1;
    readyQ[1] = thread2;    

    // load the value of rsp of thread1 into the kernel rsp
    // this will cause context switch

   __asm__("cli");
    asm volatile("movq %0, %%cr3":: "a"(thread1.cr3));
   __asm__("sti");
        

    print("I am in process virtual address space \n");

  //  __asm__ __volatile__("cli");
    __asm__ __volatile__ (
            "movq %0, %%rsp;" //load next's stack in rsp
            :
            :"r"(thread1.rsp)
    );

        __asm__ __volatile__( "popq %r11");
        __asm__ __volatile__( "popq %r10");
        __asm__ __volatile__( "popq %r9");
        __asm__ __volatile__( "popq %r8");
        __asm__ __volatile__( "popq %rdi");
        __asm__ __volatile__( "popq %rsi");
       __asm__ __volatile__( "popq %rdx"); 
        __asm__ __volatile__( "popq %rcx"); 
        __asm__ __volatile__( "popq %rbx");
        __asm__ __volatile__( "popq %rax");

    tss.rsp0=(uint64_t)&thread1.stack[63];
    
    asm volatile("mov $0x2b,%ax");
    asm volatile("ltr %ax");
  
     __asm__ __volatile__("iretq");
}


void switch_to(task* prev, task* next) {
    // will save the content prev task on the stack
    // will update the value of the current rsp to the point to the rsp of the next task
    // this will cause the context switch from prev task to next task
    // TODO: write the assembly language code

 /*   __asm__ __volatile__ (
            "pushq %rax;"\
            "pushq %rbx;"
            "pushq %rcx;"\
            "pushq %rdx;"
      8  );
   */ __asm__ __volatile__(
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
  /*  __asm__ __volatile__(
            "popq %rdx;"\
            "popq %rcx;"\
            "popq %rbx;"\
            "popq %rax;"
    );  
*/

    tss.rsp0=(uint64_t)&next->stack[63];

  //  asm volatile("mov $0x2b,%ax");
 //  __asm__("hlt"); 
//  asm volatile("ltr %ax");
        __asm__ __volatile__( "popq %r11");
        __asm__ __volatile__( "popq %r10");
        __asm__ __volatile__( "popq %r9");
        __asm__ __volatile__( "popq %r8");
        __asm__ __volatile__( "popq %rdi");
        __asm__ __volatile__( "popq %rsi");
       __asm__ __volatile__( "popq %rdx"); 
        __asm__ __volatile__( "popq %rcx"); 
        __asm__ __volatile__( "popq %rbx");
        __asm__ __volatile__( "popq %rax");

    


    __asm__ __volatile__("iretq");
//   __asm__ __volatile__("jmp next");
}
/*
void schedule() {
    // will halt the currently executing thread
    // will pop the next task from the ready queue
    // call switch_to function with prev and next task
    
    if(sflag) {
        sflag = false;
        switch_to(&readyQ[0], &readyQ[1]);
    }
    else {
        sflag = true;
        switch_to(&readyQ[1], &readyQ[0]);
    }

}
*/
void function1() {
  //      print("\nHello2");

   while(1) {
        print("\nHello");
     //   schedule();
       yield();
   }
}

void function2() {
   //     print("World..!!");
    while(1) {
         print("World..!!");
       // schedule();
        yield();
     }
}
