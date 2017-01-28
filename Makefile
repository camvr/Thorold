ASM=nasm
CC=gcc
VM=qemu-system-x86_64

BOOT=src/boot
KERNEL=src/kernel
BOOTFILE=src/image

all: boot disk test

boot:
	$(ASM) $(BOOT).asm -f bin -o $(BOOT).bin
	$(CC) -ffreestanding -c $(KERNEL).cpp -o $(KERNEL).bin
	
	
disk: boot
	dd if=/dev/zero of=$(BOOTFILE).bin bs=512 count=2880
	dd if=$(BOOT).bin of=$(BOOTFILE).bin conv=notrunc
	dd if=$(KERNEL).bin of=$(BOOTFILE).bin conv=notrunc bs=512 seek=1
	
test:
	$(VM) -drive format=raw,file=$(BOOTFILE).bin,index=0,if=floppy

clean:
	cd src && rm *.bin