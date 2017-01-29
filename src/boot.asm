bits 32
section .text
	; Multiboot header
	align 4
	dd 0x1BADB002
	dd 0x00
	dd - (0x1BADB002 + 0x00)

global start
extern kernel_main

; calling the kernel
start:
	cli
	mov esp, stack_alloc
	call kernel_main
	hlt

section .bss
resb 8192
stack_alloc:
