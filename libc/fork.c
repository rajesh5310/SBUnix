
#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>

pid_t fork()
{
   uint64_t a=__syscall0(4);
   //__syscall0(4);
   //printf("[print=%d]",a);
    //return __syscall0(4);
    return a;
}


