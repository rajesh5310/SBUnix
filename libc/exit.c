
#include <syscall.h>
#include <stdio.h>
  
void exit(int status) {
     __syscall1(5, status);   
}

