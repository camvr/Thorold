#ifndef KERNEL_H
#define KERNEL_H

#include "common.hpp"
#include "video.hpp"
#include "keyboard_map.hpp"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INT_GATE 0x8E
#define KERNEL_CODE_SEG_OFFSET 0x08


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

/* KERNEL METHOD SIGNATURES */
void idt_init(void);
void kb_init(void);
void kernel_init(void);

#endif
