#include <defs.h>
#include <stdlib.h>
#include <syscall.h>

int write(file *fd, char * buf, int size)
{
    return __syscall3(17, (uint64_t)fd, (uint64_t)buf, (uint64_t)size);
}
