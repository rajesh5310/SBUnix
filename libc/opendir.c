#include <defs.h>
#include <stdlib.h>
#include <syscall.h>

uint64_t opendir(char* dir) {
    uint64_t addr = __syscall1(12, (uint64_t)dir);
    return addr;
}
