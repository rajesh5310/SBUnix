#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <defs.h>

#define SYSCALL_PROTO(n) static __inline uint64_t __syscall#n
#define T_SYSCALL   128

static __inline uint64_t __syscall0(uint64_t n) {

     uint64_t val;
    __asm volatile("movq %1,%%rax;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n)
                   :"rbp","rcx","rbx","rdx","rsi","rdi","r12","r11","r10","r9","r8","r13","r14","r15"//Clobberred registers
                   );  
    return val;

}

static __inline uint64_t __syscall1(uint64_t n, uint64_t a1) {
/*    uint64_t res;
     __asm__ volatile ("int %1"\
                   :"=a"(res)\
                   :"i"(T_SYSCALL),"0"(n),"b"((uint64_t)(a1))\
                   :"rax","rbx");
  return res;
*/
      uint64_t val;                                                                                                                          
    __asm volatile("movq %1,%%rax;"
                   "movq %2,%%rbx;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n),"r"(a1)
                   :"rbp","rcx","rdx","rsi","rdi","r12","r11","r10","r9","r8","r13","r14","r15"//Clobberred registers
                  );
   return val;

}

static __inline uint64_t __syscall2(uint64_t n, uint64_t a1, uint64_t a2) {
    uint64_t res;
    __asm__ volatile ("int %1"\
                    :"=a"(res)\
                    :"i"(T_SYSCALL),"0"(n) ,"b"((uint64_t)(a1)),"c"((uint64_t)(a2))\
                    :"cc","memory");
    return res;
}

static __inline uint64_t __syscall3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
    uint64_t res;
    __asm__ volatile ("int %1"\
                    :"=a"(res)\
                    :"i"(T_SYSCALL),"0"(n),"b"((uint64_t)(a1)),"c"((uint64_t)(a2)),"d"((uint64_t)(a3))\
                    :"cc","memory");
    return res;
}

static __inline uint64_t __syscall4(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
    uint64_t res;
    __asm__ volatile ("int %1"\
                    :"=a"(res)\
                    :"i"(T_SYSCALL),"0" (n),"b"((uint64_t)(a1)),"c"((uint64_t)(a2)),"d"((uint64_t)(a3)),"D"((uint64_t)(a4))\
                    :"cc","memory");
    return res;
}


/*
SYSCALL_PROTO(0)(uint64_t n) {
    uint64_t __res;
     __asm__ volatile ("int %1"\
                :"=a"(__res)\
                :"i"(T_SYSCALL),"0"(n)\
                :"cc","memory");
  return __res;
}

SYSCALL_PROTO(1)(uint64_t n, uint64_t a1) {
    uint64_t __res;
     __asm__ volatile ("int %1"\
                   :"=a"(__res)\
                   :"i"(T_SYSCALL),"0"(n),"b"((uint64_t)(a1))\
                   :"cc","memory");
  return __res;
}

SYSCALL_PROTO(2)(uint64_t n, uint64_t a1, uint64_t a2) {
    uint64_t __res;
    __asm__ volatile ("int %1"\
                    :"=a"(__res)\
                    :"i"(T_SYSCALL),"0"(n) ,"b"((uint64_t)(a1)),"c"((uint64_t)(a2))\
                    :"cc","memory");
    return __res;
}

SYSCALL_PROTO(3)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
    uint64_t __res;
    __asm__ volatile ("int %1"\
                    :"=a"(__res)\
                    :"i"(T_SYSCALL),"0"(n),"b"((uint64_t)(a1)),"c"((uint64_t)(a2)),"d"((uint64_t)(a3))\
                    :"cc","memory");
    return __res;
}

SYSCALL_PROTO(4)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
    uint64_t __res;
    __asm__ volatile ("int %1"\
                    :"=a"(__res)\
                    :"i"(T_SYSCALL),"0" (n),"b"((uint64_t)(a1)),"c"((uint64_t)(a2)),"d"((uint64_t)(a3)),"D"((uint64_t)(a4))\
                    :"cc","memory");
    return __res;
}
*/
//#endif




























#endif
