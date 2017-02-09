#include "kernel.hpp"

/* KERNEL METHOD DECLARATIONS */

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
// initialize kernel
void kernel_init(void)
{
	// default kernel variables
	// Initialize video
	init_video();

	// welcome message
        kprint("vmOS", get_color(DGREY,BLACK));
        printnl();
        printCmdPrompt();
        // init io
        idt_init();
        kb_init();

	update_cursor();
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
			// handleKey
		}
		else if (keycode == 0x4D) {	// Right arrow key TODO fix delete chars
			// handleKey	
		}
		else {
			// display typed character
			putChar(keyboard_map[(unsigned char) keycode]);
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

