#include <stdlib.h>
int main(int argc, char* argv[], char* envp[]) {

		int x = 0;
	    printf("\ntest %x", &x);
	    exit(1);
	    //asm("int $0x80");
	    while(1);
	    return 0;
}
