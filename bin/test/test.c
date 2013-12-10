#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[]) {
	int i;
    for(i = 0; i< 5; i++)
        printf("Execvped process. Before Sleep. \n");

    sleep(5);

    for(i = 0; i< 5; i++)
        printf(" Execvped process. After Sleep.\n");
    
    exit(-1);
    return 0;
}
