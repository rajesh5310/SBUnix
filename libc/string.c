
#include <string.h>

/*void *
memset(void *v, uint64_t c, size_t n)
{

    if (n == 0)
        return v;
    if ((uint64_t)v%4 == 0 && n%4 == 0) {
        c &= 0xFF;
        c = (c<<24)|(c<<16)|(c<<8)|c;
        asm volatile("cld; rep stosl\n"
            :: "D" (v), "a" (c), "c" (n/4)
            : "cc", "memory");
    } else
        asm volatile("cld; rep stosb\n"
            :: "D" (v), "a" (c), "c" (n)
            : "cc", "memory");
    return v;
}*/
