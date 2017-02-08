ASM=nasm
CC=g++
CFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
VM=qemu-system-i386
KVERSION=001

BOOT=src/boot
KERNEL=src/kernel
KBIN=kernel
LINKER=src/link.ld
BOOTFILE=src/image

all: boot disk test

boot:
	$(ASM) -f elf $(BOOT).asm -o $(BOOT).o
	$(CC) -m32 -c $(KERNEL).cpp -o $(KERNEL).o $(CFLAGS)
	ld -m elf_i386 -T $(LINKER) -o $(KBIN) $(BOOT).o $(KERNEL).o

disk: boot
	#grub-mkrescue iso --output=kernel.iso
	
test:
	$(VM) -kernel $(KBIN)

clean:
	cd src && rm *.o
	rm $(KBIN)
