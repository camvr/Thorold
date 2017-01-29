ASM=nasm
CC=g++
VM=qemu-system-x86_64

BOOT=src/loader
KERNEL=src/kernel
LINKER=src/linker.ld
BOOTFILE=src/image

all: boot disk test

boot:
	$(ASM) -f elf $(BOOT).asm -o $(BOOT).o
	$(CC) -c $(KERNEL).cpp -ffreestanding -nostdlib -fno-builtin -fno-rtti -fno-exceptions
	LDEMULATION="elf_x86_64"
	ld -T $(LINKER) -o $(KERNEL) $(BOOT).o $(KERNEL).o
	
disk: boot
	cp $(KERNEL) iso/boot/kernel
	grub-mkrescue iso --output=kernel.iso 
	
test:
	$(VM) -cdrom kernel.iso

clean:
	cd src && rm *.o *.bin
