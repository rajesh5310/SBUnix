#include <stdio.h>
#include <defs.h>
#include <stdlib.h>
int main(int argc, char* argv[], char* envp[]) {

    printf("\n\ninside ls %x", argv[0]);

    exit(-1);
    return 0;
} 
