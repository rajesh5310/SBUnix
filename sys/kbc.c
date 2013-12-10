/*
 * kbc.c
 *
 *  Created on: 9 Aug 2011
 *      Author: Chris Stones
 */

#include <defs.h>
#include <sys/ports.h>
#include <sys/print.h>
#include <sys/kbc.h>
#include <sys/kbc_scancodes.h>
#include <string.h>

char keyboard_buf[100];
volatile int is_taking_input = 0;
int keyboard_buf_count = 0;
uint64_t video_ptr_kb;
extern volatile char * video_memory; 
void reset_keyboard_buf()
{
    int i = 0;
    for(i = 0; i < 100; i++)
        keyboard_buf[i] = '\0';
}

/*
 * Routine that handles keyboard interrupt
 */
void kbc_irq() {

    static uint8_t shift = 0;
    static uint8_t ctrl= 0;

    // Read scan from port 0x60
    uint8_t b = port_inb(0x60);

    //Detect if shift is pressed
    switch(b)
    {
        case KBCSC_LEFTSHIFT:
            shift |= 1;
            break;
        case KBCSC_RIGHTSHIFT:
            shift |= 2;
            break;
        case (KBCSC_LEFTSHIFT | 0x80):
            shift &= ~1;
            break;
        case (KBCSC_RIGHTSHIFT | 0x80):
            shift &= ~2;
            /*case (KBCSC_LEFTCTRL | 0x80):
              ctrl |= 1;
              break;
              case (KBCSC_LEFTCTRL):
              ctrl |= 1;
              break;*/
            /*case (KBCSC_RIGHTCTRL | 0x80):
              shift &= ~2;*/

        default:
            break;
    }

    //Call method that converts scan code to character
    uint8_t c = get_tochar(b, shift, ctrl);
    if(c!=0)
    {
        print("%c", c);
        print_line(1,24, "%c", c);
    }

}

/*
 * Method that prints character as per the shift value
 */
static uint8_t char_shift_adjust(uint8_t c, uint8_t shift) {

    return c + (shift ? ('A'-'a') : 0);
}

/*
 * Method that converts scan code to character
 */
uint8_t get_tochar(uint8_t b, uint8_t shift , uint8_t ctrl) {

    char ch = '\0';

    if(b==KBCSC_SPACE)
        ch = ' ';

    if(b==KBCSC_RETURN)
    {
        keyboard_buf[keyboard_buf_count++] = '\0';
        keyboard_buf_count = 0;
        is_taking_input = 0;    
        //print("asd\n");
    }
    if(b==KBCSC_BACKSPACE)
    {
        if((uint64_t)video_memory > video_ptr_kb)
        {
            keyboard_buf[keyboard_buf_count] = '\0';
            keyboard_buf_count--;
            print_backspace();
        }
    }
    if((b >= KBCSC_Q) && (b <= KBCSC_P))
    {
        ch = char_shift_adjust("qwertyuiop"[b-KBCSC_Q], shift);
    }


    if((b >= KBCSC_A) && (b <= KBCSC_L))
        ch = char_shift_adjust("asdfghjkl"[b-KBCSC_A], shift);

    if((b >= KBCSC_Z) && (b <= KBCSC_M))
        ch = char_shift_adjust("zxcvbnm"[b-KBCSC_Z], shift);

    if((b >= KBCSC_1) && (b <= KBCSC_9)) {
        if(shift)
            ch = "!@#$%^&*("[b-KBCSC_1];
        ch = '1' + b-KBCSC_1;
    }

    if((b >= KBCSC_OPEN_CURL) && (b <= KBCSC_CLOSE_CURL)) {
        if(shift)
            ch = "{}"[b-KBCSC_OPEN_CURL];
        ch = "[]"[b-KBCSC_OPEN_CURL];
    }

    if((b >= KBCSC_COLON) && (b <= KBCSC_TILDA)) {
        if(shift)
            ch = ":\"~"[b-KBCSC_COLON];
        ch = ";'`"[b-KBCSC_COLON];
    }

    if((b >= KBCSC_COMMA) && (b <= KBCSC_SLASH)) {
        if(shift)
            ch = "<>?"[b-KBCSC_COMMA];
        ch = ",./"[b-KBCSC_COMMA];
    }


    if((b >= KBCSC_UNDERSCORE_MINUS) && (b <= KBCSC_EQUALS_PLUS)) {
        if(shift)
            ch = "_+"[b-KBCSC_UNDERSCORE_MINUS];
        ch = "-="[b-KBCSC_UNDERSCORE_MINUS];
    }


    if(b == KBCSC_OR)
        ch = shift ? '|' : '\\';


    if(b == KBCSC_0)
        ch = shift ? ')' : '0';

    if(is_taking_input == 1 && ch != '\0')
    {
        keyboard_buf[keyboard_buf_count++] = ch;
        //print_line(10, 10 ,"%d %d", keyboard_buf_count, ch);
    }
    return ch;
}

void scanf(char * buf)
{
    video_ptr_kb = (uint64_t)video_memory;
    is_taking_input = 1;
    while(is_taking_input == 1);
    //    print("done");
    strcpy(buf, keyboard_buf);
    reset_keyboard_buf();
}
