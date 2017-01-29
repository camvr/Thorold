#include <stddef.h>
#include <stdint.h>

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

// kernel variables
static const size_t VWIDTH = 80;
static const size_t VHEIGHT = 24;
size_t cursor_col;
size_t cursor_row;
uint8_t console_col;
uint16_t* video_mem;


// Clears the video_mem
void clear(void)
{
	size_t row;
	size_t col;
        for (row = 0; row < VHEIGHT; row++) {
                for (col = 0; col < VWIDTH; col++) {
                        video_mem[row*VWIDTH+col] = videoMemChar(' ', console_col);
                }
        }
}


// Initialize kernel
void kernel_init(void)
{
	// init kernel variables
	cursor_col = 0;
	cursor_row = 0;
	console_col = get_color(LGREY, BLACK);
	video_mem = (uint16_t*)0xB8000;
	
	// Clear video_mem
	clear();
}

// Set console colour
void setConsoleColor(uint8_t col)
{
	console_col = col;
}

// Write char to the video_mem
void printChar(char c, uint8_t col, size_t x, size_t y)
{
	video_mem[y*VHEIGHT+x] = videoMemChar(c,col);
}

void putChar(char c)
{
	printChar(c, console_col, cursor_col, cursor_row);
	if (++cursor_col == VWIDTH) {
		cursor_col = 0;
		if (++cursor_row == VHEIGHT) {
			cursor_row = 0;
		}
	}
}

// Write string to the video_mem
void printf(const char* str)
{
	size_t strLen = len(str);
	size_t i;
	for (i = 0; i < strLen; i++)
		putChar(str[i]);
}

// Main function
void kernel_main(void)
{
	// initialize the kernel
	kernel_init();

	printf("Hello, world!");
	while(1);
}

