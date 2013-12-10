#include <defs.h>                                                                                         
#include <stdlib.h>
#include <syscall.h>
void  ps()
{
   __syscall0(21);
}
