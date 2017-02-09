#include "kernel.hpp"

/* KERNEL METHOD DECLARATIONS */

void outb(uint16_t port, uint8_t val)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (val));
}

uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

// Setup the IDT
void idt_init(void)
{
        unsigned long kb_addr;
        unsigned long idt_addr;
        unsigned long idt_ptr[2];
        kb_addr = (unsigned long)key_handler;
        IDT[0x21].offset_low  = kb_addr & 0xFFFF;
        IDT[0x21].sel         = KERNEL_CODE_SEG_OFFSET;
        IDT[0x21].zero        = 0;
        IDT[0x21].type_attr   = INT_GATE;
        IDT[0x21].offset_high = (kb_addr & 0xFFFF0000) >> 16;

        // ICW1 - Initialize
        outb(0x20, 0x11);
        outb(0xA0, 0x11);

        // ICW2 - Remap offset address of IDT
        outb(0x21, 0x20);
        outb(0xA1, 0x28);

        // ICW3 - setup cascading
        outb(0x21, 0x00);
        outb(0xA1, 0x00);

        // ICW4 - enviroment info
        outb(0x21, 0x01);
        outb(0xA1, 0x01);

        // mask interrupts
        outb(0x21, 0xFF);
        outb(0xA1, 0xFF);

        idt_addr   = (unsigned long)IDT;
        idt_ptr[0] = (sizeof (struct idt_entry) * IDT_SIZE) + ((idt_addr & 0xFFFF) << 16);
        idt_ptr[1] = idt_addr >> 16;

        load_idt(idt_ptr);
}

// Initialize keyboard
void kb_init(void)
{
	outb(0x21, 0xFD);
}

// Returns the color code of given fg and bg

uint8_t get_color(enum color fg, enum color bg)
{
	return fg | bg << 4;
}

// Create a 16 bit integer for writing to video memory
uint16_t videoMemChar(char c, uint8_t color)
{
	uint16_t ch  = c;
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
	uint16_t tcursor_loc   = (cursor_row * VWIDTH) + cursor_col;
	uint8_t ch	       = video_buffer[(size_t)tcursor_loc*2];
	uint8_t col	       = video_buffer[(size_t)(tcursor_loc*2)+1];
	video_mem[tcursor_loc] = videoMemChar(ch, col);
	outb(0x3D4, 14);
	outb(0x3D5, tcursor_loc >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, tcursor_loc);
}

// Clears the video_mem
void clear(void)
{
	size_t i = 0;
	while (i < SCREENSIZE) {
		video_buffer[i] = ' ';
		video_mem[i++]  = ' ';
		video_buffer[i] = console_col;
		video_mem[i++]  = console_col;
	}
}


// Initialize kernel
void kernel_init(void)
{
	// default kernel variables
	cursor_col  = 0;
	cursor_row  = 0;
	console_col = get_color(LGREY, BLACK);
	minCol      = len(cmd_prompt);
	video_mem   = (uint16_t*)0xB8000;
	
	// Clear video_mem
	clear();

	// welcome message
        kprint("vmOS", get_color(DGREY,BLACK));
        printnl();
        kprint(cmd_prompt, get_color(RED,BLACK));
        // init io
        idt_init();
        kb_init();

	update_cursor();
}

// Set console colour
void setConsoleColor(uint8_t col)
{
	console_col = col;
}

// Handling scrolling the screen
void scrollScreen(void)
{
	size_t pos    = 0;
	size_t offset = VWIDTH;
	cursor_col    = 0;
	cursor_row    = VHEIGHT - 1;
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
}

// Write char to the video_mem
void printChar(char c, uint8_t col, size_t x, size_t y)
{
	uint16_t loc      = (y*VWIDTH)+x;
	video_buffer[loc] = videoMemChar(c,col);
	video_mem[loc]    = video_buffer[loc];
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
	kprint(cmd_prompt, get_color(RED,BLACK));
}

// Handles task of backspace
void handleBackspace(void)
{
	if (--cursor_col < minCol)
		cursor_col = minCol;
	printChar(' ', console_col, cursor_col, cursor_row);
}

// Main keyboard handler
extern "C" void key_handler_main(void)
{
	unsigned char status;
	char keycode;

	// write EOI
	outb(0x20, 0x20);

	status = inb(KEYBOARD_STATUS_PORT);
	if (status & 0x01) {
		keycode = inb(KEYBOARD_DATA_PORT);
		if (keycode <= 0) {
		}			
		else if (keycode == 0x1C) {
			handleEnter();
		}
		else if (keyboard_map[(unsigned char) keycode] == 0x08) {
			handleBackspace();
		}
		else if (keycode == 0x4B) {	// Left arrow key TODO fix delete chars
			if (cursor_col > minCol) --cursor_col;
		}
		else if (keycode == 0x4D) {	// Right arrow key TODO fix delete chars
			if (cursor_col < VWIDTH) ++cursor_col;
		}
		else {
			// display typed character
			putChar(keyboard_map[(unsigned char) keycode], console_col);
		}
		update_cursor();
	}
}


// Main function
extern "C" void kernel_main(void)
{
	
	// initialize the kernel
	kernel_init();
	
	// hang the kernel
	while(1);
}

