/*
 * process.h
 *
 *  Created on: Nov 7, 2013
 *      Author: rajesh
 */
//PCB structure
struct task_struct{
	uint64_t state;   //0 runnable ,-1 unrunnable, >0 stopped
	uint64_t counter; //how long its run;
	uint64_t pid;    //my id;
	uint64_t priority; // when should it run
	uint64_t stack[1024]; // its own stack
	uint64_t *rsp; // process stack pointer
	uint64_t *bumpPtr; // allocation pointer in virtual address space
	uint64_t cr3; // will remove this
};
