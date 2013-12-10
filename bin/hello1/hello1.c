#include <defs.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[], char* envp[]) {
    //printf("\nProcess[%d], name[%s], [%x]\n", 2, "P2", 8765);
   //printf("P2");
   uint64_t *test = (uint64_t *)malloc(sizeof(uint64_t));
   *test = 1;
 //   while(1){
//    int i = 0;
  //  sleep(20);
 
   char *a[]={"-a","-b","-c"};
   char *b[]={"dummy","value","pass"};
   execvpe("bin/test",a,b);
    //for(i=1; i<200; i++)                                                                                                                    
   while(1)
        printf("\n P2 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  //      }
   
    exit(-1);
    return 0;
}
