/*
 * =====================================================================================
 *
 *       Filename:  syscall.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/21/2013 01:57:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Er. Udit Gupta (ukg), udit.gupta@stonybrook.edu
 *        Company:  Stony Brook University
 *
 * =====================================================================================
 */


#include <defs.h>
#include <syscall.h>
#include <sys/idt.h>
#include <sys/isr.h>
#include <stdio.h>
   
void __exit();
void __putchar(uint64_t c);

void syscall_common_handler(struct isr_stack_frame* s) {
//     int d=current_task->pid;
  //   printf("Value %d\n",d);

    int i;
   // asm("\t movq %%rax,%0" : "=r"(i));

    i=s->rax;

    if(i == 1){
   //call the exit system call
        __exit();
    }
    else if(i == 2){
     //call for printf
        printf("Value %d\n",s->rbx);
         __putchar(s->rbx);
    }
 
    else if(i == 3){
    //call for maybe fork
    }
 
 }
 
void __putchar(uint64_t c) {
    printf("I AM IN PUTCHAR\n");
 };
  
void __exit() {
    //first i need to implement free() for freeing memory
    //free vma's
    printf("I AM here\n");
     //free mm_struct
 
  //we can free task_struct here or its better to do it in wait as mentioned in the mail thread..but bcz of time left we will do it right here 
}















































