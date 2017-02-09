#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <stdint.h>
#include "keyboard_map.hpp"

#define VWIDTH 80
#define VHEIGHT 25
#define SCREENSIZE 2 * VWIDTH * VHEIGHT

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INT_GATE 0x8E
#define KERNEL_CODE_SEG_OFFSET 0x08

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

enum textMode
{
        PUSH,
        INSERT
};

struct gdt_entry
{
        // TODO implement me
};

struct idt_entry
{
        unsigned short int offset_low;
        unsigned short int sel;
        unsigned char zero;
        unsigned char type_attr;
        unsigned short int offset_high;
};

/* KERNEL VARIABLES AND CONSTS */

//struct gdt_entry GDT[];
struct idt_entry IDT[IDT_SIZE];

extern unsigned char keyboard_map[128];
extern "C" void key_handler(void);
extern "C" void load_idt(unsigned long *idt_p);

uint16_t cursor_col = 0;
uint16_t cursor_row = 0;
uint8_t console_col = 0x0;
uint16_t* video_mem = (uint16_t*)0xB8000;
uint16_t* video_buffer;
char* cmd_prompt = "$ ";
size_t minCol;
char* command_in;

/* KERNEL METHOD SIGNATURES */
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
void idt_init(void);
void kb_init(void);
uint8_t get_color(enum color fg, enum color bg);
uint16_t videoMemChar(char c, uint8_t color);
size_t len(const char* string);
void update_cursor(void);
void clear(void);
void kernel_init(void);
void setConsoleColor(uint8_t col);
void scrollScreen(void);
void printnl(void);
void printChar(char c, uint8_t col, size_t x, size_t y);
void putChar(char c, uint8_t col);
void kprint(const char* str, uint8_t col);
void handleEnter(void);
void handleBackspace(void);

#endif

