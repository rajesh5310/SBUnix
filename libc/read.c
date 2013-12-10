#include <stdio.h>
#include <defs.h>
#include <syscall.h>
#include <stdlib.h>

int read(file *fd, int size, char* buf) {
    int size_read = (int)__syscall3(15, (uint64_t)fd, (uint64_t) size, (uint64_t)buf);
    return size_read;
}
