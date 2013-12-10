#ifndef ARCH_KBC_SCANCODES_H
#define ARCH_KBC_SCANCODES_H

#include <defs.h>

enum kbsc {

    KBCSC_ESC 	= 0x01,

    KBCSC_1 	= 0x02,
    KBCSC_2 	= 0x03,
    KBCSC_3 	= 0x04,
    KBCSC_4 	= 0x05,
    KBCSC_5 	= 0x06,
    KBCSC_6 	= 0x07,
    KBCSC_7 	= 0x08,
    KBCSC_8 	= 0x09,
    KBCSC_9 	= 0x0a,
    KBCSC_0 	= 0x0b,

    KBCSC_UNDERSCORE_MINUS 		= 0x0c,
    KBCSC_EQUALS_PLUS 			= 0x0d,
    KBCSC_BACKSPACE 			= 0x0e,
    KBCSC_TAB		 			= 0x0f,

    KBCSC_Q		= 0x10,
    KBCSC_W		= 0x11,
    KBCSC_E		= 0x12,
    KBCSC_R		= 0x13,
    KBCSC_T		= 0x14,
    KBCSC_Y		= 0x15,
    KBCSC_U		= 0x16,
    KBCSC_I		= 0x17,
    KBCSC_O		= 0x18,
    KBCSC_P		= 0x19,

    KBCSC_OPEN_CURL  = 0x1a,
    KBCSC_CLOSE_CURL = 0x1b,

    KBCSC_RETURN = 0x1c,
    KBCSC_CTRL = 0x1d,

    KBCSC_A = 0x1e,
    KBCSC_S = 0x1f,
    KBCSC_D = 0x20,
    KBCSC_F = 0x21,
    KBCSC_G = 0x22,
    KBCSC_H = 0x23,
    KBCSC_J = 0x24,
    KBCSC_K = 0x25,
    KBCSC_L = 0x26,

    KBCSC_COLON = 0x27,
    KBCSC_AT 	= 0x28,

    KBCSC_TILDA	= 0x29,

    KBCSC_LEFTSHIFT	= 0x2a,
    KBCSC_BACKSLASH	= 0x2b,

    KBCSC_Z = 0x2c,
    KBCSC_X = 0x2d,
    KBCSC_C = 0x2e,
    KBCSC_V = 0x2f,
    KBCSC_B = 0x30,
    KBCSC_N = 0x31,
    KBCSC_M = 0x32,

    KBCSC_LT = 0x33,
    KBCSC_GT = 0x34,
    KBCSC_FORWARDSLASH = 0x35,
    KBCSC_RIGHTSHIFT = 0x36,
    KBCSC_PRINTSCREEN = 0x37,
    KBCSC_ALT = 0x38,
    KBCSC_SPACE = 0x39,
    KBCSC_CAPSLOCK = 0x3a,
    KBCSC_F1 = 0x3b,
    KBCSC_F2 = 0x3c,
    KBCSC_F3 = 0x3d,
    KBCSC_F4 = 0x3e,
    KBCSC_F5 = 0x3f,
    KBCSC_F6 = 0x40,
    KBCSC_F7 = 0x41,
    KBCSC_F8 = 0x42,
    KBCSC_F9 = 0x43,
    KBCSC_F10= 0x44,

    KBCSC_NUMLOCK= 0x45,
    KBCSC_SCROLLLOCK= 0x46,
    KBCSC_HOME= 0x47,
    KBCSC_UP= 0x48,
    KBCSC_PGUP= 0x49,
    KBCSC_NUMPAD_MINUS = 0x4a,
    KBCSC_NUMPAD_4 = 0x4b,
    KBCSC_NUMPAD_5 = 0x4c,
    KBCSC_NUMPAD_6 = 0x4d,
    KBCSC_NUMPAD_PLUS = 0x4e,
    KBCSC_NUMPAD_1 = 0x4f,
    KBCSC_NUMPAD_2 = 0x50,
    KBCSC_NUMPAD_3 = 0x51,
    KBCSC_NUMPAD_0 = 0x52,
    KBCSC_NUMPAD_PERIOD = 0x53,

    KBCSC_F11= 0x57,
    KBCSC_F12= 0x58,
    KBCSC_COMMA = 0x33,
    KBCSC_SLASH = 0x35,
    KBCSC_MAX = 0x60,
    KBCSC_OR = 0x2B
};

uint8_t kbcsc_tochar(uint8_t b, uint8_t shift);

#endif /*** ARCH_KBC_SCANCODES_H ***/


