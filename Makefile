ASM=nasm
CC=gcc
VM=qemu-system-i386
KVERSION=001

BOOT=src/boot
KERNEL=src/kernel
LINKER=src/linker.ld
BOOTFILE=src/image

all: boot disk test

boot:
	$(ASM) -f elf32 $(BOOT).asm -o $(BOOT).o
	$(CC) -m32 -c $(KERNEL).c -o $(KERNEL).o
	ld -m elf_i386 -T $(LINKER) -o kernel $(BOOT).o $(KERNEL).o

disk: boot
	mv kernel kernel-$(KVERSION)
	#grub-mkrescue iso --output=kernel.iso
	
test:
	$(VM) -kernel kernel-$(KVERSION)

clean:
	cd src && rm *.o
	rm kernel
