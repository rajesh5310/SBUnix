#include <defs.h>
#include <stdlib.h>
#include <syscall.h>
file* open(char* file_name) {
    uint64_t addr = __syscall1(14, (uint64_t)file_name);
    if(addr == 0)
        return NULL;
    return (file *) addr;
}
