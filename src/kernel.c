#include "terminal.h"
#include "keyboard_map.h"


/* KERNEL DEFINITIONS */

#define VWIDTH 80
#define VHEIGHT 25
#define SCREENSIZE 2 * VWIDTH * VHEIGHT

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INT_GATE 0x8E
#define KERNEL_CODE_SEG_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C


/* KERNEL VARIABLES AND CONSTS */

enum color
{
        BLACK = 0x0,
        BLUE = 0x1,
        GREEN = 0x2,
        CYAN = 0x3,
        RED = 0x4,
        MAGENTA = 0x5,
        BROWN = 0x6,
        LGREY = 0x7,
        DGREY = 0x8,
        LBLUE = 0x9,
        LGREEN = 0xA,
        LCYAN = 0xB,
        LRED = 0xC,
        LMAGENTA = 0xD,
        LBROWN = 0xE,
        WHITE = 0xF
};

struct idt_entry
{
	unsigned short int offset_low;
	unsigned short int sel;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_high;
};

struct idt_entry IDT[IDT_SIZE];

extern unsigned char keyboard_map[128];
extern void key_handler(void);
extern char rport(unsigned short port);
extern void wport(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_p);

size_t cursor_col = 0;
size_t cursor_row = 0;
uint8_t console_col = 0x0;
uint16_t* video_mem = (uint16_t*)0xB8000;
char* command_in;

/* KERNEL BASE FUNCTIONS */

// Setup the IDT
void idt_init(void)
{
        unsigned long kb_addr;
        unsigned long idt_addr;
        unsigned long idt_ptr[2];
        kb_addr = (unsigned long)key_handler;
        IDT[0x21].offset_low = kb_addr & 0xFFFF;
        IDT[0x21].sel = KERNEL_CODE_SEG_OFFSET;
        IDT[0x21].zero = 0;
        IDT[0x21].type_attr = INT_GATE;
        IDT[0x21].offset_high = (kb_addr & 0xFFFF0000) >> 16;

        // ICW1 - Initialize
        wport(0x20, 0x11);
        wport(0xA0, 0x11);

        // ICW2 - Remap offset address of IDT
        wport(0x21, 0x20);
        wport(0xA1, 0x28);

        // ICW3 - setup cascading
        wport(0x21, 0x00);
        wport(0xA1, 0x00);

        // ICW4 - enviroment info
        wport(0x21, 0x01);
        wport(0xA1, 0x01);

        // mask interrupts
        wport(0x21, 0xFF);
        wport(0xA1, 0xFF);

        idt_addr = (unsigned long)IDT;
        idt_ptr[0] = (sizeof (struct idt_entry) * IDT_SIZE) + ((idt_addr & 0xFFFF) << 16);
        idt_ptr[1] = idt_addr >> 16;

        load_idt(idt_ptr);
}

// Initialize keyboard
void kb_init(void)
{
	wport(0x21, 0xFD);
}

// Returns the color code of given fg and bg

uint8_t get_color(enum color fg, enum color bg)
{
	return fg | bg << 4;
}

// Create a 16 bit integer for writing to video memory
uint16_t videoMemChar(char c, uint8_t color)
{
	uint16_t ch = c;
	uint16_t col = color;
	return ch | col << 8;
}

// Returns the length of a string
size_t len(const char* string)
{
	size_t length = 0;
	while (string[length] != 0)
		length++;
	return length;
}

// Update the text mode cursor position
void update_cursor(void)
{
	uint16_t tcursor_loc = 2;//(uint16_t)(cursor_row * VWIDTH) + cursor_col;
	wport(0x3D4, 14);
	wport(0x3D5, tcursor_loc >> 8);
	wport(0x3D4, 15);
	wport(0x3D5, tcursor_loc);
}

// Clears the video_mem
void clear(void)
{
	size_t i = 0;
	while (i < SCREENSIZE) {
		video_mem[i++] = ' ';
		video_mem[i++] =  console_col;
	}
}


// Initialize kernel
void kernel_init(void)
{
	// default kernel variables
	cursor_col = 0;
	cursor_row = 0;
	console_col = get_color(LGREY, BLACK);
	video_mem = (uint16_t*)0xB8000;
	
	update_cursor();
	// Clear video_mem
	clear();
}

// Set console colour
void setConsoleColor(uint8_t col)
{
	console_col = col;
}

// Handling scrolling the screen
void scrollScreen(void)
{
	size_t pos = 0;
	size_t offset = VWIDTH;
	cursor_col = 0;
	cursor_row = VHEIGHT - 1;
	while (pos < SCREENSIZE) {
		if (pos >= SCREENSIZE-(VWIDTH*2)) {
			video_mem[pos++] = ' ';
			video_mem[pos++] = console_col;
		} else {
			video_mem[pos++] = video_mem[offset++];
			video_mem[pos++] = video_mem[offset++];
		}
	}
}

// Goes to newline
void printnl(void)
{
	cursor_col = 0;
	if (++cursor_row == VHEIGHT)
		scrollScreen();
	update_cursor();
}

// Write char to the video_mem
void printChar(char c, uint8_t col, size_t x, size_t y)
{
	video_mem[(y*VWIDTH)+x] = videoMemChar(c,col);
}

void putChar(char c, uint8_t col)
{
	printChar(c, col, cursor_col, cursor_row);
	if (++cursor_col == VWIDTH) {
		cursor_col = 0;
		if (++cursor_row == VHEIGHT) {
			cursor_row = 0;
		}
	}
	update_cursor();
}

// Write string to the video_mem
void kprint(const char* str, uint8_t col)
{
	size_t strLen = len(str);
	size_t i;
	for (i = 0; i < strLen; i++)
		putChar(str[i], col);
}

// Handles task of enter key
void handleEnter(void)
{
	printnl();

	// Command line
	// call command parser
	//kprint(command_in, console_col);
	printnl();
	kprint("$ ", get_color(RED,BLACK));
}

// Handles task of backspace
void handleBackspace(void)
{
	if (--cursor_col == 1)
		cursor_col = 2;
	video_mem[(cursor_row*VWIDTH)+cursor_col] = videoMemChar(' ', console_col);
}

// Main keyboard handler
void key_handler_main(void)
{
	unsigned char status;
	char keycode;

	// write EOI
	wport(0x20, 0x20);

	status = rport(KEYBOARD_STATUS_PORT);
	if (status & 0x01) {
		keycode = rport(KEYBOARD_DATA_PORT);
		if (keycode < 0)
			return;

		if (keycode == ENTER_KEY_CODE) {
			handleEnter();
			return;
		}

		if (keyboard_map[(unsigned char) keycode] == 0x08) {
			handleBackspace();
			return;
		}
		
		// display typed character
		putChar(keyboard_map[(unsigned char) keycode], console_col);
		
	}
}


// Main function
void kernel_main(void)
{
	// initialize the kernel
	kernel_init();
	// welcome message
	kprint("vmOS", get_color(DGREY,BLACK));
	printnl();
	kprint("$ ", get_color(RED,BLACK));
	// init io
	idt_init();
	kb_init();

	// hang the kernel
	while(1);
}

