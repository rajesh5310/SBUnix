/*
 * =====================================================================================
 *
 *       Filename:  process.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/2013 02:33:24 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Er. Udit Gupta (ukg), udit.gupta@stonybrook.edu
 *        Company:  Stony Brook University
 *
 * =====================================================================================
 */


#ifndef _PROCESS_H
#define _PROCESS_H

#include <defs.h>
#include <sys/pmap.h>

struct vm_area_struct{                                                                                   
    struct mm_struct * vm_mm;       /* associated mm_struct */
    uint64_t vm_start;              /* VMA start, inclusive */
    uint64_t vm_end;                /* VMA end, exclusive */
    uint64_t vm_mmsz;               /* VMA size */ 
    unsigned long vm_flags;         /* flags */
    uint32_t grows_down;
    uint64_t vm_file;          /* mapped file, if any */
    struct vm_area_struct  *vm_next;/* list of VMA's */ 
    uint64_t vm_offset;    /* file offset */  
};


struct mm_struct {
    int count;
    uint64_t * pt; // page table pointer  
    unsigned long context;
    unsigned long start_code, end_code, start_data, end_data;
    unsigned long start_brk, brk, start_stack, start_mmap;
    unsigned long arg_start, arg_end, env_start, env_end;
    unsigned long rss, total_vm, locked_vm;
    unsigned long def_flags;
    struct vm_area_struct * mmap;
    struct vm_area_struct * mmap_avl;
};


struct vm_area_struct *malloc_vma(struct mm_struct *mm);
//struct task_struct * malloc_pcb(struct task_struct *,void *fun_ptr);
struct task_struct * malloc_pcb(void *fun_ptr);

/****************** Process **********************/ 
//PCB structure
struct task_struct{
    uint64_t state;   //0 runnable ,-1 unrunnable, >0 stopped   
    uint64_t counter; //how long its run;
    uint64_t pid;    //my id;
    uint64_t ppid;  //my parent pid   
    uint64_t priority; // when should it run
    uint64_t *stack; // its own stack 
    uint64_t *rsp; // process stack pointer                                                        
    uint64_t pml4e; // will directly go to cr3 register 
    uint64_t cr3;
    uint64_t *bmpPtr; // user stack of the process
    struct mm_struct *mm; // a pointer to mm_struct
    uint64_t rip; // instruction pointer
    uint64_t status; // exit status
    uint64_t zombie; // mark as zombie process
    uint64_t kstack[512];   
    struct vm_area_struct *heap_vma;  // vma for heap
    int sleep_time;
    uint64_t entry;
    int is_waiting;
    char pname[100]; //process name
};

void run_process(struct task_struct **pcb);    
uint64_t ready_queue[100];  // can accomodate 100 process now 
uint64_t zombie_queue[100] ; // queue of all process which contains exit status of zombie process 
uint64_t sleep_queue[100];  // To keep track the time quantum 


void sleep_process();

void exit_process(int i);  // exit process 
int get_pid();
int get_ppid();   
struct task_struct* get_current_process();
void* malloc(uint64_t  size);   
int fork();  
void fork_process(); 
int sleep(int j);   


uint64_t sleeping_process_count;  // keeps track the number of sleeping process 
uint64_t process_count; // keep tracks the number of current process in ready_queue(run_queue)  

void schedule();

#define yield() asm("int $0x80")

void * get_kva(struct Page *pp);

void function1();                                                                                           
void function2();

void load_icode(struct task_struct *t, char *filename, size_t size);

static __inline void lcr3(uint64_t val) {
    __asm __volatile("movq %0,%%cr3" : : "r" (val));
}


uint64_t sleeping_process_count;  // keeps track the number of sleeping process 
uint64_t process_count; // keep tracks the number of current process in ready_queue(run_queue)
struct vm_area_struct *malloc_vma(struct mm_struct *mm);

void region_alloc(struct task_struct *t, void *va, size_t len);

int get_pid();
int get_ppid();
int fork();

void exit_process(int status);

int sleep(int t);

void*  malloc(uint64_t  size);

uint64_t execvpe(char *arg1,char *arg2[], char* arg3[]);
int waitpid(int pid, int status) ;

void dp();
void kshutdown();
#endif
