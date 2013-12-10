#include <defs.h>
#include <syscall.h>
#include <stdlib.h>
void* malloc(uint64_t size)
{
  return (void *)(__syscall1(7, size));
}                                                                                                                                              
