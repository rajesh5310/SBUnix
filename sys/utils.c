#include <defs.h>
#include <sys/print.h>                                                                                                                             
#include <sys/tarfs.h>
#include <string.h>
#include <utils.h>
///#include <utils.h>
long stoi(const char *s) 
{
    long i;
    i = 0;
    while(*s >= '0' && *s <= '9')
    {   
        i = i * 10 + (*s - '0');
        s++;
    }   
    return i;
}
uint64_t power (uint64_t x, int e) {
    if (e == 0) return 1;

    return x * power(x, e-1);
}

uint64_t octalToDecimal(uint64_t octal)
{
    uint64_t decimal = 0, i=0;
    while(octal!=0){
        decimal = decimal + (octal % 10) * power(8,i++);
        octal = octal/10;
    }   
    return decimal;
}

