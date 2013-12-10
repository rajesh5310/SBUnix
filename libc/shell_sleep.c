#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
void shell_sleep(uint64_t ticks)
{
    __syscall1(22, (uint64_t)ticks);
}
