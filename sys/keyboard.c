/* @name : keyboard.c
 * @author : rgolani, abmishra, skandalamsri
 * @last updated date : 25th September, 2013
 */

#include <defs.h>
#include <ports.h>
#include <print.h>
#include <keyboard.h>

/*
* Routine that handles keyboard interrupt
*/
void handle_keyboard_irq() {

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
        print_line(1,23, "%c", c);
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


	if(b==KBCSC_SPACE)
		return ' ';

	if(b==KBCSC_RETURN)
		print("\n");

	if(b==KBCSC_BACKSPACE)
		print_backspace();

	if((b >= KBCSC_Q) && (b <= KBCSC_P))
	{
        return char_shift_adjust("qwertyuiop"[b-KBCSC_Q], shift);
	}


	if((b >= KBCSC_A) && (b <= KBCSC_L))
		return char_shift_adjust("asdfghjkl"[b-KBCSC_A], shift);

	if((b >= KBCSC_Z) && (b <= KBCSC_M))
		return char_shift_adjust("zxcvbnm"[b-KBCSC_Z], shift);

	if((b >= KBCSC_1) && (b <= KBCSC_9)) {
		if(shift)
			return "!@#$%^&*("[b-KBCSC_1];
		return '1' + b-KBCSC_1;
	}

    if((b >= KBCSC_OPEN_CURL) && (b <= KBCSC_CLOSE_CURL)) {
		if(shift)
			return "{}"[b-KBCSC_OPEN_CURL];
		return "[]"[b-KBCSC_OPEN_CURL];
	}

	if((b >= KBCSC_COLON) && (b <= KBCSC_TILDE)) {
		if(shift)
			return ":\"~"[b-KBCSC_COLON];
		return ";'`"[b-KBCSC_COLON];
	}

	if((b >= KBCSC_COMMA) && (b <= KBCSC_SLASH)) {
		if(shift)
			return "<>?"[b-KBCSC_COMMA];
		return ",./"[b-KBCSC_COMMA];
	}


	if((b >= KBCSC_UNDERSCORE_MINUS) && (b <= KBCSC_EQUALS_PLUS)) {
		if(shift)
			return "_+"[b-KBCSC_UNDERSCORE_MINUS];
		return "-="[b-KBCSC_UNDERSCORE_MINUS];
	}


    if(b == KBCSC_OR)
		return shift ? '|' : '\\';


	if(b == KBCSC_0)
		return shift ? ')' : '0';

	return 0;
}

