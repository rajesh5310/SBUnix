#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

uint64_t readdir(uint64_t dir) {
    uint64_t ret = __syscall1(13, (uint64_t)(dir));
    return ret;
}
