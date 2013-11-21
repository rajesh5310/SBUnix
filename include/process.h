/*
 * =====================================================================================
 *
 *       Filename:  process.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/12/2013 03:49:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rajesh Golani
 *        Company:  Stony Brook University
 *
 * =====================================================================================
 */

#ifndef _PROCESS_H
#define _PROCESS_H

#include <defs.h>

typedef struct {
    uint64_t stack[1024];       // process local stack
    uint64_t rsp;           // stack pointer
    uint64_t cr3;           // current value of CR3 register for process switch
    uint64_t* pml4e;
    uint64_t entry;
} task;



void first_cs();
void schedule();
#endif

