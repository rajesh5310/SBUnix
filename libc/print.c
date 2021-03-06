/* @name : print.c
 * @author : rgolani, abmishra, skandalamsri
 * @last updated date : 25th September, 2013
 */
#include "defs.h"
#include "print.h"
#include "ports.h"
#define WHITE_TXT 0x0f // white on black text
#define COLOR 0x09
#define VIDEO_MEM 0xb8000


#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
//#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;

void clear_screen();
int putchar(char** ch, volatile char** video_memory);
char * convert_to_string(unsigned int value, char * str, int base );
void move_cursor(int x, int y, volatile char ** vidmem );
void put_line_feed(volatile char ** vidmen);
volatile char *vid_memory = (char *) 0xb8000;
int print_line(int x, int y, char *message, ...);
int putchar_color(char** ch, int colour,  volatile char** video_memory);

volatile char * video_memory =  (volatile char *) (VIDEO_MEM + 160);

/*
* Handles new line (\n)
*/
void put_line_feed(volatile char ** vidmen)
{
    int curr_loc = (unsigned long int)*vidmen;
    int start_loc = (unsigned long int)0xB8000;
    int offset = 160 - (curr_loc - start_loc)%160;
    *vidmen = *vidmen + offset;
}

/*
* Moves video memory pointer to point (x,y)
*/
void move_cursor(int x, int y, volatile char ** vidmem )
{
    int offset = 0;
    int line_factor = 160;
    offset = y*line_factor + x*2;
    *vidmem = *vidmem + offset;
}

/*
* Clears the screen
*/
void clear_screen()
{
    volatile char *vidmem =  (char *)VIDEO_MEM;
    unsigned int i=0;
    while(i < (80*25*2))
    {
        vidmem[i]=' ';
        i++;
        vidmem[i]=WHITE_TXT;
        i++;
    };
}

/*
* Handles print function
*/
int print(char *message, ...)
{
    char* message_local;
    char* temp_str;
    int temp_int;
    char temp_char;
    va_list vl;
    void *temp_m;
    int p_add;

    va_start(vl, message);
    message_local = message;
    while( *message_local != 0 )
    {

        if(*message_local != 37 && *message_local != '\n')
        {
            putchar(&message_local, &video_memory);
            message_local++;
            continue;
        }

        if(*message_local == '\n')
        {
            put_line_feed(&video_memory);
            message_local++;
            continue;
        }

        switch(*++message_local)
        {
            case 's':
                temp_str = NULL;
                temp_str = va_arg(vl, char *);
                while(*temp_str) {
                    putchar(&temp_str, &video_memory);
                    temp_str++;
                }
                message_local = message_local + 1;
                break;
            case 'd':
                temp_str = NULL;
                temp_int = va_arg(vl, int);
                convert_to_string(temp_int, temp_str, 10);
                while(*temp_str) {
                    putchar(&temp_str, &video_memory);
                    temp_str++;
                }
                message_local = message_local + 1;
                break;
            case 'x':
                temp_str = NULL;
                temp_int = va_arg(vl, int);
                convert_to_string(temp_int, temp_str, 16);
                while(*temp_str) {
                    putchar(&temp_str, &video_memory);
                    temp_str++;
                }
                message_local = message_local + 1;
                break;
            case 'c':
                temp_str = NULL;
                temp_char = va_arg(vl, int);
                temp_str = &temp_char;
                putchar(&temp_str, &video_memory);
                message_local = message_local + 1;
                break;
            case 'p':
                temp_str = NULL;
                temp_m =  va_arg(vl,  void *);
                p_add = (long unsigned int) temp_m;
                convert_to_string(p_add, temp_str, 16);
                while(*temp_str) {
                    putchar(&temp_str, &video_memory);
                    temp_str++;
                }
                message_local = message_local + 1;
                break;
            }
    }
    va_end(vl);
    update_cursor_current_loc();
    return 0;
}

/*
* Prints the content on secified location i.e. (x,y)
*/
int print_line(int x, int y, char *message, ...)
{
    volatile  char *video_mem;
    char* message_local;
    char* temp_str;
    video_mem = (char *)(0xB8000);
    int temp_int;
    char temp_char;
    va_list vl;
    void *temp_m;
    int p_add;

    move_cursor (x, y, &video_mem);

    va_start(vl, message);
    message_local = message;
    while( *message_local != 0 )
    {

        if(*message_local != 37 && *message_local != '\n')
        {
            putchar_color(&message_local, 0x0b , &video_mem);
            message_local++;
            continue;
        }

        if(*message_local == '\n')
        {
            put_line_feed(&video_mem);
            message_local++;
            continue;
        }

        switch(*++message_local)
        {
            case 's':
                temp_str = NULL;
                temp_str = va_arg(vl, char *);
                while(*temp_str) {
                    putchar(&temp_str, &video_mem);
                    temp_str++;
                }
                message_local = message_local + 1;
                break;
            case 'd':
                temp_str = NULL;
                temp_int = va_arg(vl, int);
                convert_to_string(temp_int, temp_str, 10);
                while(*temp_str) {
                    putchar_color(&temp_str, 0x06,&video_mem);
                    temp_str++;
                }
                message_local = message_local + 1;
                break;
            case 'x':
                temp_str = NULL;
                temp_int = va_arg(vl, int);
                convert_to_string(temp_int, temp_str, 16);
                while(*temp_str) {
                    putchar(&temp_str, &video_mem);
                    temp_str++;
                }
                message_local = message_local + 1;
                break;
            case 'c':
                temp_str = NULL;
                temp_char = va_arg(vl, int);
                temp_str = &temp_char;
                putchar(&temp_str, &video_mem);
                message_local = message_local + 1;
                break;
            case 'p':
                temp_str = NULL;
                temp_m =  va_arg(vl,  void *);
                p_add = (long unsigned int) temp_m;
                convert_to_string(p_add, temp_str, 16);
                while(*temp_str) {
                    putchar(&temp_str, &video_mem);
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
* Handles the keyboard backspace button
*/
void print_backspace()
{
    int curr_loc = (unsigned long int)video_memory;
    int start_loc = (unsigned long int)0xB8000;
    if ((curr_loc - start_loc) > 160)
    {
        video_memory -= 2;
        *video_memory = '\0';
        video_memory += 1;
        *video_memory = WHITE_TXT;
        video_memory += 1;
        video_memory -= 2;
        update_cursor_current_loc();
    }
}

/*
* Updates the cursor to (x,y)
*/
void update_cursor(int row, int col)
{
    unsigned short position=(row*80) + col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}
/*
* Updates cusor to current location
*/
void update_cursor_current_loc()
{
    int curr_loc = (unsigned long int)video_memory;
    int start_loc = (unsigned long int)0xB8000;

    unsigned short position= (curr_loc - start_loc)/2;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

/*
* Function that prints character to screen
*/
int putchar_color(char** ch, int colour, volatile char** video_memory)
{
    **video_memory = **ch;
    *video_memory += 1;
    **video_memory = colour;
    *video_memory += 1;
    return 0;
}

/*
* Function that prints character to screen
*/
int putchar(char** ch, volatile char** video_memory)
{
    **video_memory = **ch;
    *video_memory += 1;
    **video_memory = WHITE_TXT;
    *video_memory += 1;
    return 0;
}

/*
* Converts number of any base to string
*/
char * convert_to_string(unsigned int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}


