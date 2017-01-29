ASM=nasm
CC=gcc
VM=qemu-system-i386
KVERSION=001

BOOT=src/boot
KERNEL=src/kernel
KBIN=kernel
LINKER=src/linker.ld
BOOTFILE=src/image

all: boot disk test

boot:
	$(ASM) -f elf32 $(BOOT).asm -o $(BOOT).o
	$(CC) -m32 -c $(KERNEL).c -o $(KERNEL).o
	ld -m elf_i386 -T $(LINKER) -o $(KBIN) $(BOOT).o $(KERNEL).o

disk: boot
	#grub-mkrescue iso --output=kernel.iso
	
test:
	$(VM) -kernel $(KBIN)

clean:
	cd src && rm *.o
	rm $(KBIN)
