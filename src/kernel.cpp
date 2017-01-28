void start()
{
	unsigned char* vga = (unsigned char*) 0xB8000;
	vga[0] = 'X';
	vga[1] = 0x09;

	// main loop
	while (true) {

	}
}