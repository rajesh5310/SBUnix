#include <defs.h>
#include <stdlib.h>
#include <syscall.h>

int waitpid(int pid,int status) {                                                                                                              
 return  __syscall2(20,pid, status);
}

