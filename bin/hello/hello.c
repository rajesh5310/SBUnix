#include <stdio.h>
#include <defs.h>
#include <stdlib.h>
int j;
int main(int argc, char* argv[], char* envp[]) {
    //    uint64_t *mem;
    //printf("\nP1");
  //  j = fork();
   char buf[1024] = "asdsd assda sasads assadsad";
   printf("%s", buf);
   
   file *fd = open("hdd/test1234.txt");
   write(fd, buf, 30);
   //int size = open("hdd/test3.txt"), 32, (uint64_t) buf);
    printf("\nSize of %s", fd->filename);
   //printf("\n %s", buf);
    //seek(fd, 900, SEEK_END);
    //seek(fd, 100, SEEK_CUR);
    //seek(fd, 512, SEEK_SET);
    char* test = (char *)malloc(1000*sizeof(char));
    read(fd, 100, test);
    printf(" \n offset  %d %s \n",fd->offset, test);

   //while(1)
    /*uint64_t add = opendir("bin/");
    printf("%x", add);
    readdir(add);
    */
    /*int i = 0 ;
    printf("enter no ");
    scanf("%d", &i);
    printf("%d", i);
*/
    /*char test[10];
    printf("\n %x", test);
    printf("enter string");
    scanf("%s", test);
    printf("%s", test);
*/
   // printf("[[[[[[[[[[[[]]]]]]]]]]]]value of j==%d[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]",j);
    //printf("[[[[[[[[[[[[]]]]]]]]]]]]value of j==%d[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]",j);
   /* if(j==0) { 
        while(1)
          ps();
          // printf("I am a child of %d with id %d\n", getppid(), getpid());   
        
    }
    else {
    //printf("[[[[[[[[[[[[]]]]]]]]]]]]value of j==%d[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]",j);
        while(1)
            readdir("bin/");
            //ps();
            //printf("\nparent");
            printf("I am a parent with id %d\n" , getpid());   

    }
*/
    //    mem=malloc(512);
    //    printf("\nmem=[%x]\n",mem); 
 while(1){
//        printf("P1");
        };
 //   exit(1);
    return 0;
}
