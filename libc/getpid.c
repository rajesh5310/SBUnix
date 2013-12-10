#include <defs.h>
#include <stdlib.h>
#include <syscall.h>                                                                                                                           
pid_t getpid()
{
  return __syscall0(8);
}
