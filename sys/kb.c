#include <sys/interrupts.h>
#include <sys/kprintf.h>

//control lines of KB-shift,caps,ctrl

unsigned char shift = 0;            // Shift Key Status 
unsigned char ctrl = 0;             // Ctrl Key Status
unsigned char alt = 0;              // Alt Key Status 
unsigned char caps = 0;             // Caps Lock Status
unsigned char num = 0;              // Num Lock Status 

unsigned char asciiCode;            // The ASCII Code

unsigned char keyBuff[257];         // Keybuffer 
volatile unsigned char keyBuffEnd = 0;  // The Last key in the buffer

unsigned char echoON = 1;           // Echo keys

unsigned char kbScanCodes[512] =    
    /// Keyboard character maps (Look-up table)
{					
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[' , ']', '\n', 0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* Shifted */
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(' , ')', '_' , '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{' , '}', '\n', 0,
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'', '~', 0,
	'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, '7', '8', '9', 0, '4', '5', '6', 0,
	'1', '2', '3', '0', 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* Caps, Normal */
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{' , '}', '\n', 0,
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'', '~', 0,
	'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* Caps, Shifted */
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(' , ')', '_' , '+', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[' , ']', '\n', 0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*********************************************************************************************/
//Handle special keys like Shift, ctrl, alt, num, caps

int kb_special(unsigned char key)
{
	static int specKeyUp = 1;	// Is a key already been or being pressed?
	switch(key) 
	{
		case 0x36: // R-Shift down
		case 0x2A: // L-Shift down
			shift = 1;
			break;
		case 0xB6: // R-Shift up
		case 0xAA: // L-Shift up
			shift = 0;
			break;
		case 0x1D: // Control down
			ctrl = 1;
			break;
		case 0x9D: // Control up
			ctrl = 0;
			break;
		case 0x38: // Alt down
			alt = 1;
			break;
		case 0xB8: // Alt up
			alt = 0;
			break;
		case 0x3A: // Caps down
			if(specKeyUp == 1) 
			{
				caps = !caps;
				specKeyUp = 0;
			}
			break;
		case 0x45: // Num down
			if(specKeyUp == 1)
			{
				num = !num;
				specKeyUp = 0;
			}
			break;
		case 0x46: // Scroll down
			if(specKeyUp == 1)
			{
				num = !num;
				specKeyUp = 0;
			}
			break;
		case 0xBA: // Caps Up
		case 0xC5: // Num Up
		case 0xC6: // Scroll Up
			specKeyUp = 1;
			break;
		case 0xE0:
			break;
		default:
			return(0);
	}
	return (1);
}

/**********************************************************************************/

//Handle keyboard interrupt

void keyboard_handler(struct isr_regs *r)
    /// Get keyboard input during the keyboard interrupt
    /// Convert the scancode into ascii data.
//void keyboard_handler()
{

unsigned char scanCode;
scanCode = inb(0x60);
unsigned char asciiCode;

if(!(kb_special(scanCode) | (scanCode >= 0x80)))
{
                if(shift)		//Determine key that has been pressed
		{
			if(!caps)
			{
				asciiCode = kbScanCodes[scanCode + 128];
			}
			else
			{
				asciiCode = kbScanCodes[scanCode + 384];
			}
		}
		else
		{
			if(!caps)
			{
				asciiCode = kbScanCodes[scanCode];
			}
			else
			{
				asciiCode = kbScanCodes[scanCode + 256];
			}
		}

	        keyBuffEnd++;
		keyBuff[keyBuffEnd] = asciiCode;
		if (echoON){
			for(int i = 0; i!=keyBuffEnd;i++)
			kprintf("%c",keyBuff[i]);

		}

}
}

/*
void keyboard_install()
    /// Install the Keyboard handler into the IDT
{
    //FlushBuffer();                      // Anything in the buffer is crap
	interrupt_handler(&keyboard_handler);  // Install Handler
}
*/
/*
char getchar()
    /// Get a charecter from the stdin
{
	int i = 0;
	while(keyBuffEnd == 0);

	for(; i < keyBuffEnd; i++)
	{
		keyBuff[i] = keyBuff[i + 1];
	}
	keyBuffEnd--;

	return keyBuff[0];
}
*/

void keyboard_install()
{
uint16_t scancode = 0;

//Read the status
	//uint16_t status = inb(0x64);

if((scancode = inb(0x60))> 0)
kprintf("%s","Intrrupt");
}
