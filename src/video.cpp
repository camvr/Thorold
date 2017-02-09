#include "video.hpp"

uint8_t   cursor_col;
uint8_t   cursor_row;
uint8_t   console_col;
uint8_t   prompt_col;
uint16_t* video_mem;
uint16_t* video_buffer;
char*     cmd_prompt;
size_t    min_col;
size_t    max_col;

void init_video(void)
{
	cursor_col  = 0;
	cursor_row  = 0;
	console_col = get_color(LGREY,BLACK);
	prompt_col  = get_color(RED,BLACK);
	video_mem   = (uint16_t*)0xB8000;
	cmd_prompt  = "$ ";
        min_col     = len(cmd_prompt);
	max_col     = min_col+1; // TODO change me!
	clear();
}

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
        uint8_t ch             = video_buffer[(size_t)tcursor_loc*2];
        uint8_t col            = video_buffer[(size_t)(tcursor_loc*2)+1];
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
void kprint(const char* str, uint8_t col)
{
        size_t strLen = len(str);
        size_t i;
        for (i = 0; i < strLen; i++)
                putChar(str[i], col);
}

void printCmdPrompt(void)
{
	kprint(cmd_prompt, prompt_col);
}

// Handles task of enter key
void handleEnter(void)
{
        printnl();

        // Command line
        // call command parser
        //kprint(command_in, console_col);
        printnl();
	printCmdPrompt();
}

// Handles task of backspace
void handleBackspace(void)
{
        if (--cursor_col < min_col)
                cursor_col = min_col;
        printChar(' ', console_col, cursor_col, cursor_row);
}

