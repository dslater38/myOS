#include <stdio.h>
#include "common.h"
#include "isr.h"
#include <array>


struct Key
{
	uint8_t code;
	uint8_t shiftCode;
	uint8_t ctrlCode;
	uint8_t altCode;
};

std::array<Key, 128> usKeys {
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },
	Key{.code = '\x27', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },
	Key{.code = '1', .shiftCode = '!', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '2', .shiftCode = '@', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '3', .shiftCode = '#', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '4', .shiftCode = '$', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '5', .shiftCode = '%', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '6', .shiftCode = '^', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '7', .shiftCode = '&', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '8', .shiftCode = '*', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '9', .shiftCode = '(', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '0', .shiftCode = ')', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '-', .shiftCode = '_', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '=', .shiftCode = '+', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '\b', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },
	Key{.code = '\t', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'q', .shiftCode = 'Q', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'w', .shiftCode = 'W', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'e', .shiftCode = 'E', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'r', .shiftCode = 'R', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 't', .shiftCode = 'T', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'y', .shiftCode = 'Y', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'u', .shiftCode = 'U', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'i', .shiftCode = 'I', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'o', .shiftCode = 'O', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'p', .shiftCode = 'P', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '[', .shiftCode = '{', .ctrlCode = 0, .altCode = 0 },
	Key{.code = ']', .shiftCode = '}', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '\n', .shiftCode = '\n', .ctrlCode = 0, .altCode = 0 }, // Return
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 }, // Control Left
	Key{.code = 'a', .shiftCode = 'A', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 's', .shiftCode = 'S', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'd', .shiftCode = 'D', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'f', .shiftCode = 'F', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'g', .shiftCode = 'G', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'h', .shiftCode = 'H', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'j', .shiftCode = 'J', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'k', .shiftCode = 'K', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'l', .shiftCode = 'L', .ctrlCode = 0, .altCode = 0 },
	Key{.code = ';', .shiftCode = ':', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '\'', .shiftCode = '\"', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '`', .shiftCode = '~', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Shift Left
	Key{.code = '\\', .shiftCode = '|', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'z', .shiftCode = 'Z', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'x', .shiftCode = 'X', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'c', .shiftCode = 'C', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'v', .shiftCode = 'V', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'b', .shiftCode = 'B', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'n', .shiftCode = 'N', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 'm', .shiftCode = 'M', .ctrlCode = 0, .altCode = 0 },
	Key{.code = ',', .shiftCode = '<', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '.', .shiftCode = '>', .ctrlCode = 0, .altCode = 0 },
	Key{.code = '/', .shiftCode = '?', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Shift Right
	Key{.code = '*', .shiftCode = '*', .ctrlCode = 0, .altCode = 0 },	// KP_multiply
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Alt
	Key{.code = ' ', .shiftCode = ' ', .ctrlCode = 0, .altCode = 0 },
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// CtrlL_Lock
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F1
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F2
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F3
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F4
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F5
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F6
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F7
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F8
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F9
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F10
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// NumLock
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Scroll Lock
	Key{.code = '7', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_7
	Key{.code = '8', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_8
	Key{.code = '9', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_9
	Key{.code = '-', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_Subtract
	Key{.code = '4', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_4
	Key{.code = '5', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_5
	Key{.code = '6', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_6
	Key{.code = '+', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_ADD
	Key{.code = '1', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_1
	Key{.code = '2', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_2
	Key{.code = '3', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_3
	Key{.code = '0', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_0
	Key{.code = '.', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_Period
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Last_Console
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// less
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F11
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F12
	Key{.code = '\n', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_Enter
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Control
	Key{.code = '/', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_Divide
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// 
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Alt
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Break
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Find
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Up
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Prior
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Left
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Right
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Select
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Down
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Insert
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Remove
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Macro
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F13
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F14
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Help
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Do
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// F17
	Key{.code = '-', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_Minus
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Pause
	Key{.code = '.', .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// KP_Period
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Alt
	Key{.code = 0, .shiftCode = 0, .ctrlCode = 0, .altCode = 0 },	// Alt
};


namespace {
/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
	unsigned char kbdus[128] =
	{
		0, 27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
		'9', '0', '-', '=', '\b',	/* Backspace */
		'\t',			/* Tab */
		'q', 'w', 'e', 'r',	/* 19 */
		't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
		0,			/* 29   - Control */
		'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
		'\'', '`', 0,		/* Left shift */
		'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
		'm', ',', '.', '/', 0,					/* Right shift */
		'*',
		0,	/* Alt */
		' ',	/* Space bar */
		0,	/* Caps lock */
		0,	/* 59 - F1 key ... > */
		0, 0, 0, 0, 0, 0, 0, 0,
		0,	/* < ... F10 */
		0,	/* 69 - Num lock*/
		0,	/* Scroll Lock */
		0,	/* Home key */
		0,	/* Up Arrow */
		0,	/* Page Up */
		'-',
		0,	/* Left Arrow */
		0,
		0,	/* Right Arrow */
		'+',
		0,	/* 79 - End key*/
		0,	/* Down Arrow */
		0,	/* Page Down */
		0,	/* Insert Key */
		0,	/* Delete Key */
		0, 0, 0,
		0,	/* F11 Key */
		0,	/* F12 Key */
		0,	/* All other keys are undefined */
	};
}

void keyboard_interrupt_handler(registers64_t regs)
{
	unsigned char scancode = 0;

    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
	    printf("%c", kbdus[scancode]);
    }
}

void init_keyboard_handler()
{
	register_interrupt_handler64(IRQ1, keyboard_interrupt_handler);
}
