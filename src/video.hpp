#ifndef VIDEO_H
#define VIDEO_H

#include "common.hpp"

#define VWIDTH 80
#define VHEIGHT 25
#define SCREENSIZE 2 * VWIDTH * VHEIGHT

enum color
{
        BLACK    = 0x0,
        BLUE     = 0x1,
        GREEN    = 0x2,
        CYAN     = 0x3,
        RED      = 0x4,
        MAGENTA  = 0x5,
        BROWN    = 0x6,
        LGREY    = 0x7,
        DGREY    = 0x8,
        LBLUE    = 0x9,
        LGREEN   = 0xA,
        LCYAN    = 0xB,
        LRED     = 0xC,
        LMAGENTA = 0xD,
        LBROWN   = 0xE,
        WHITE    = 0xF
};


void init_video();
uint8_t get_color(enum color fg, enum color bg);
uint16_t videoMemChar(char c, uint8_t color);
size_t len(const char* string);
void update_cursor(void);
void clear(void);
void setConsoleColor(uint8_t col);
void scrollScreen(void);
void printnl(void);
void printChar(char c, uint8_t col, size_t x, size_t y);
void putChar(char c, uint8_t col);
void putChar(char c);
void kprint(const char* str, uint8_t col);
void printCmdPrompt(void);
void handleEnter(void);
void handleBackspace(void);

#endif
