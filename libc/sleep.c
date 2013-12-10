#include <defs.h>
#include <stdlib.h>
#include <syscall.h>
uint64_t sleep(uint64_t time)
{
  return (uint64_t)__syscall1(6, time);
}
