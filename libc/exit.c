#include <stdlib.h>
#include <syscall.h>
  
void exit(int status) {
           __syscall0(5);  //EXIT CALL NUMBER SHOULD BE 1
}

