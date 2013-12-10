
#include <stdio.h>
#include <defs.h>
#include <stdlib.h>
int main(int argc, char* argv[], char* envp[]) {
    
    char *str = (char *)malloc(10*sizeof(char)); 
    
    printf("Enter string : ");
    scanf("%s", str);
    printf("\n String is %s", str);
    exit(1);
    return 0;
}

