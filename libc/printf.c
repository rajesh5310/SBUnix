
#include <syscall.h>

int putchar_user(char ch);
#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
//#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;
/*
* Converts number of any base to string
*/
char * convert_to_string_user(uint64_t value, char * str, int base )
{
	//int length=0,start, end;
	int j, length = 0;
	//char c;
	char temp[30];
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }

    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *str++ = '-';
    }
    // Remember where the numbers start.
    //low = str;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        temp[length++] = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    temp[length] = '\0';
    for(j= length-1; j >=0 ; j--)
    {
    	*str++ = temp[j];
    }
    *str = '\0';
    return str;
}

/*
* Handles print function
*/
int printf(char *message, ...)
{
    char* message_local;

    uint64_t temp_int;
    char temp_char;
    va_list vl;
    void *temp_m;
    uint64_t p_add;
    uint64_t count;

    va_start(vl, message);
    message_local = message;
    while( *message_local != 0 )
    {

        if(*message_local != 37 && *message_local != '\n')
        {
            putchar_user(*message_local);
            message_local++;
            continue;
        }

        if(*message_local == '\n')
        {
        	putchar_user(*message_local);
            message_local++;
            continue;
        }

        switch(*++message_local)
        {
        	char temp_str_i[1024], *temp_str;
            case 's':
                //temp_str = NULL;
                temp_str = va_arg(vl, char *);
                while(*temp_str) {
                    putchar_user(*temp_str);
                    temp_str++;
                }
                message_local = message_local + 1;
                break;
            case 'd':
                temp_int = va_arg(vl, uint64_t);
                convert_to_string_user(temp_int, temp_str_i, 10);
                count = 0;
                while(temp_str_i[count]!='\0') {
                    putchar_user(temp_str_i[count]);
                    count++;
                }
                message_local = message_local + 1;
                break;
            case 'x':
                temp_str = NULL;
            	temp_int = (uint64_t)va_arg(vl, uint64_t);
				convert_to_string_user(temp_int, temp_str_i, 16);
				count = 0;
				while(temp_str_i[count]!='\0') {
					putchar_user(temp_str_i[count]);
					count++;
				}
				message_local = message_local + 1;
				break;
            case 'c':
                temp_str = NULL;
                temp_char = va_arg(vl, uint64_t);
                temp_str = &temp_char;
                putchar_user(*temp_str);
                message_local = message_local + 1;
                break;
            case 'p':
                temp_str = NULL;
                temp_m =  va_arg(vl,  void *);
                p_add = (uint64_t) temp_m;
                convert_to_string_user(p_add, temp_str, 16);
                while(*temp_str) {
                    putchar_user(*temp_str);
                    temp_str++;
                }
                message_local = message_local + 1;
                break;
            }
    }
    va_end(vl);
    return 0;
}

/*
* Function that prints character to screen
*/
int putchar_user(char ch)
{
	__syscall1(2, (uint64_t)ch);
	return 0;
}

