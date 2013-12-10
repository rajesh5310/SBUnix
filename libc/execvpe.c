#include <defs.h>
#include <stdlib.h>
#include <syscall.h>                                                                                                                           
int execvpe (const char *filename, char *const argv[], char *const env[])
{
  return __syscall3(10, (uint64_t)filename, (uint64_t)argv, (uint64_t)env);
}
