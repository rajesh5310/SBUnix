/* @name : print.c
 * @author : rgolani, abmishra, skandalamsri
 * @last updated date : 25th September, 2013
 */
#include "defs.h"
#include "sys/print.h"
#include "sys/ports.h"
#define WHITE_TXT 0x0f // white on black text
#define COLOR 0x09
#define VIDEO_MEM 0xFFFFFFFF800B8000
#define TOTAL_LINE 22
//#define VIDEO_MEM 0xB8000


#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
//#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;

void clear_screen();
int putchar(char ch, volatile char** video_memory);
char * convert_to_string(uint64_t value, char * str, int base );
void move_cursor(int x, int y, volatile char ** vidmem );
void put_line_feed(volatile char ** vidmen);
volatile char *vid_memory = (char *) VIDEO_MEM;
int print_line(int x, int y, char *message, ...);
volatile char * video_memory =  (volatile char *) (VIDEO_MEM + 160);
void itoa(int i, char b[]);
void start_scroller()
{
    /*    int i = 0;
          int j = 0;
          video_memory = (volatile char *) (VIDEO_MEM + 160);
    //    volatile char *video_memory_1 = (volatile char *) (VIDEO_MEM + 320);
    //volatile char * video_memory_1 =  (volatile char *) (VIDEO_MEM + 320);
    for(i = 1; i < TOTAL_LINE - 1; i++)
    {
    for(j = 0; j < 160; j++)
    {
     *video_memory =  *(video_memory+160);
     video_memory += 1;
     }

     }
     for(i = 0 ; i < 160; i++)
     {
     *video_memory = '\0';
     video_memory += 1;
     }
     video_memory -= 160;
     update_cursor(9, 1);*/


    volatile uint64_t i = 0;
    volatile char *vidmem = (volatile char *) (VIDEO_MEM + 160);
    volatile uint64_t pos1 = 0;
    volatile uint64_t pos2 = 0;
    volatile uint64_t var1 = 1;
    volatile uint64_t var2 = 2;

    for(var1 = 2; var1 < 23; var1++)
    {  
        for(var2 = 0; var2 < 80; var2++)
        {  
            pos1 = (((var1-1)*80) + var2)*2;
            pos2 = ((var1*80) + var2)*2;
            vidmem[pos1++] = vidmem[pos2++];
            vidmem[pos1] = vidmem[pos2];
        }  
        ///clear_line(i, 0);
    }  

    video_memory = video_memory - 160;
    for(i = 0 ; i < 160; i++)
    {  
        *video_memory = '\0';
        video_memory += 1;
    }  
    video_memory = video_memory - 160;
}

void start_scrolling()
{
    uint64_t diff = (uint64_t)video_memory - (uint64_t)0xFFFFFFFF800B8000;
    if(diff == TOTAL_LINE*160)
    {
        start_scroller();
    }

}
void panic(char *message)
{
    print(message);
    __asm__("hlt");
}
/*
 * Handles new line (\n)
 */
void puts(char *str)
{
    while(*str != '\0')
        print("%c", *str++);
}
void put_line_feed(volatile char ** vidmen)
{
    start_scrolling(vidmen);
    uint64_t curr_loc = (unsigned long int)*vidmen;
    uint64_t start_loc = (unsigned long int)VIDEO_MEM;
    uint64_t offset = 160 - (curr_loc - start_loc)%160;
    *vidmen = *vidmen + offset;
}

void change_video_pointer()
{
    clear_screen();
    video_memory =  (volatile char *)(0xFFFFFFFF800B8000 + 160);
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
    char* message_local=message;

    uint64_t temp_int;
    char temp_char;
    va_list vl;
    void *temp_m;
    uint64_t p_add;
    uint64_t count;

    va_start(vl, message);
    // message_local = message;
    while( *message_local != 0 )
    {

        if(*message_local != 37 && *message_local != '\n')
        {
            putchar(*message_local, &video_memory);
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
            char temp_str_i[1024], *temp_str;
            case 's':
            //temp_str = NULL;
            temp_str = va_arg(vl, char *);
            while(*temp_str) {
                putchar(*temp_str, &video_memory);
                temp_str++;
            }
            message_local = message_local + 1;
            break;
            case 'd':
            temp_int = va_arg(vl, uint64_t);
            convert_to_string(temp_int, temp_str_i, 10);
            count = 0;
            while(temp_str_i[count]!='\0') {
                putchar(temp_str_i[count], &video_memory);
                count++;
            }
            message_local = message_local + 1;
            break;
            case 'x':
            temp_str = NULL;
            temp_int = (uint64_t)va_arg(vl, uint64_t);
            convert_to_string(temp_int, temp_str_i, 16);
            count = 0;
            while(temp_str_i[count]!='\0') {
                putchar(temp_str_i[count], &video_memory);
                count++;
            }
            message_local = message_local + 1;
            break;
            case 'c':
            temp_str = NULL;
            temp_char = va_arg(vl, uint64_t);
            temp_str = &temp_char;
            putchar(*temp_str, &video_memory);
            message_local = message_local + 1;
            break;
            case 'p':
            temp_str = NULL;
            temp_m =  va_arg(vl,  void *);
            p_add = (uint64_t) temp_m;
            convert_to_string(p_add, temp_str_i, 16);
            count = 0;
            while(temp_str_i[count]!='\0') {
                putchar(temp_str_i[count], &video_memory);
                count++;
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

    video_mem = (char *)(0xFFFFFFFF800B8000);
    int temp_int, count;
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
            putchar(*message_local, &video_mem);
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
            char temp_str_i[1024],  *temp_str;
            case 's':
            temp_str = NULL;
            temp_str = va_arg(vl, char *);
            while(*temp_str) {
                putchar(*temp_str, &video_mem);
                temp_str++;
            }
            message_local = message_local + 1;
            break;
            case 'd':
            temp_int = va_arg(vl, int);
            convert_to_string(temp_int, temp_str_i, 10);
            count = 0;
            while(temp_str_i[count]!='\0') {
                putchar(temp_str_i[count], &video_mem);
                count++;
            }
            message_local = message_local + 1;
            break;
            case 'x':
            temp_str = NULL;
            temp_int = va_arg(vl, int);
            convert_to_string(temp_int, temp_str, 16);
            while(*temp_str) {
                putchar(*temp_str, &video_mem);
                temp_str++;
            }
            message_local = message_local + 1;
            break;
            case 'c':
            temp_str = NULL;
            temp_char = va_arg(vl, int);
            temp_str = &temp_char;
            putchar(*temp_str, &video_mem);
            message_local = message_local + 1;
            break;
            case 'p':
            temp_str = NULL;
            temp_m =  va_arg(vl,  void *);
            p_add = (long unsigned int) temp_m;
            convert_to_string(p_add, temp_str, 16);
            while(*temp_str) {
                putchar(*temp_str, &video_mem);
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
    uint64_t curr_loc = (unsigned long int)video_memory;
    uint64_t start_loc = (unsigned long int)VIDEO_MEM;
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
    uint64_t curr_loc = (unsigned long int)video_memory;
    uint64_t start_loc = (unsigned long int)VIDEO_MEM;

    unsigned short position= (curr_loc - start_loc)/2;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

/*
 * Function that prints character to screen
 */
int putchar(char ch, volatile char** video_memory)
{
    start_scrolling();
    **video_memory = ch;
    *video_memory += 1;
    **video_memory = WHITE_TXT;
    *video_memory += 1;
    return 0;
}

int putc(char ch)
{
    start_scrolling();
    if(ch == '\n')
        print("\n");
    else
    {
        *video_memory = ch;
        video_memory += 1;
        *video_memory = WHITE_TXT;
        video_memory += 1;
    }return 0;
}
int putc_color(char ch)
{
    start_scrolling();
    if(ch == '\n')
        print("\n");
    else
    {
        *video_memory = ch;
        video_memory += 1;
        *video_memory = 0x6;
        video_memory += 1;
    }return 0;
}



/*
 * Converts number of any base to string
 */
char * convert_to_string(uint64_t value, char * str, int base )
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


