#include <syscall.h>
#include <stdio.h>
  
void shutdown(int status) {                                                                                                                    
     __syscall0(25);   
}
