#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <defs.h>

#define SYSCALL_PROTO(n) static __inline uint64_t __syscall##n

SYSCALL_PROTO(0)(uint64_t n) {
	uint64_t __res;
	__asm__ volatile ("int $0x80"
			: "=a" (__res)
			: "0" (n)
	);
	return __res;
}

SYSCALL_PROTO(1)(uint64_t n, uint64_t a1) {
	uint64_t res;
	__asm__ volatile ("int %1"
			:"=a"(res)
			:"i"(0x80),"0"(n),"b"((uint64_t)(a1))
			:"cc","memory");
	return res;
}
/*
 SYSCALL_PROTO(2)(uint64_t n, uint64_t a1, uint64_t a2) {
 return 0;
 }

 SYSCALL_PROTO(3)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
 return 0;
 }

 SYSCALL_PROTO(4)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
 return 0;
 }*/

#endif
