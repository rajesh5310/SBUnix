#include <defs.h>
#include <stdlib.h>
#include <syscall.h>

int seek(file *fd, int offset, int whence)
{
    return __syscall3(16, (uint64_t)fd, (uint64_t)offset, (uint64_t)whence);
}
