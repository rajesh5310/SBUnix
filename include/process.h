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

typedef struct {
    uint64_t stack[1024];       // process local stack
    uint64_t rsp;           // stack pointer
    uint64_t cr3;           // current value of CR3 register for process switch
} task;


void first_cs();
void schedule();

#define yield() asm("int $0x80")

task readyQ[5]; 
//bool sflag=true;

void * get_kva(struct Page *pp);
void switch_to(task* prev, task* next);

task thread1;
task thread2;

void function1();                                                                                                                              
void function2();


#endif





















