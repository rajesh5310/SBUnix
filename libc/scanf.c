#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
uint64_t stoi(char *s) // the message and then the line #
{
    uint64_t i;
    i = 0;
    while(*s >= '0' && *s <= '9')
    {
        i = i * 10 + (*s - '0');
        s++;
    }
    return i;
}
int strcmp1(const char *p, const char *q) {
	while (*p || *q) {
		if (*p != *q)
			return -1;
		p++, q++;
	}
	return 0;
}




void scanf(char* str, void* buf) {
    char temp_buf[100];
    
    __syscall1(11, (uint64_t)temp_buf);
   
    if(strcmp1(str, "%s") == 0)
    {
        buf = (char *)buf;
//        printf("----- %c", temp_buf[0]);
//        printf("in");
        //buf = temp_buf;
        int i = 0;
        while(temp_buf[i] != '\0')
            *(char *)buf++ = temp_buf[i++];
        *(char *)buf = '\0';
//        printf("%d", i);
    }
    else if(strcmp1(str, "%d") == 0)
    {
        *(int *)buf = stoi(temp_buf);
    }
    else if(strcmp1(str, "%x") == 0)
    {
        //buf = temp_buf;
    }

}
