ASM=nasm
DISKW=dd
VM=qemu-system-x86_64
BOOTASM=src/boot.asm
BOOTBIN=src/boot.bin
BOOTFILE=src/boot.img

all: boot disk test

boot: bootloader.asm
	$(ASM) $(BOOTASM) -f bin -o $(BOOTBIN)
	
disk: $(BOOTBIN)
	cd src && $(DISKW) if=/dev/zero of=blrl.img bs=1024 count=1440
	cd src && $(DISKW) status=noxfer conv=notrunc if=$(BOOTBIN) of=$(BOOTFILE)
	
test:
	cd src && $(VM) -drive format=raw,file=$(BOOTFILE),index=0,if=floppy

clean:
	cd src && rm *.bin *.img 