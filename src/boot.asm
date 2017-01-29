bits 32
section .text
	; Multiboot header
	align 4
	dd 0x1BADB002
	dd 0x00
	dd - (0x1BADB002 + 0x00)

global start
global key_handler
global rport
global wport
global load_idt
extern kernel_main
extern key_handler_main

; setup io handlers
rport:
	mov edx, [esp+4]
	in al, dx
	ret

wport:
	mov edx, [esp+4]
	mov al, [esp+4+4]
	out dx, al
	ret

load_idt:
	mov edx, [esp+4]
	lidt [edx]
	sti
	ret

key_handler:
	call key_handler_main
	iretd

; calling the kernel
start:
	cli
	mov esp, stack_alloc
	call kernel_main
	hlt

section .bss
resb 8192
stack_alloc:
