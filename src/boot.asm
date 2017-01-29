[bits 32]
global start
extern _kernel_main

; Preparing for GRUB
section .mbHeader

align 0x4

; Multiboot header setup
MODULEALIGN equ 1<<0
MEMINFO     equ 1<<1
FLAGS       equ MODULEALIGN | MEMINFO
MAGIC       equ 0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

MultiBootHeader:
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

; calling the kernel
start:
	push ebx
	call _kernel_main
