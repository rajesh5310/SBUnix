#include <stdio.h>
#include <defs.h>
#include <stdlib.h>
int main(int argc, char* argv[], char* envp[]) {
    char buf[1024] = "CSE 506 was an amazing course. Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
    printf(" File contents are : %s", buf);
    
    //File write
    file *fd = open("hdd/cse506.txt");
    write(fd, buf, 30);
    
    char test[100];
    
    seek(fd, 100 , SEEK_SET);
    read(fd, 100, test);
    printf(" \n Reading after seeking 100 bytes from start :  %s \n",test);

    seek(fd, 100 , SEEK_CUR);
    read(fd, 100, test);
    printf(" \n Reading after seeking 100 bytes from end :  %s \n",test);


    exit(2);
    return 0;
}
