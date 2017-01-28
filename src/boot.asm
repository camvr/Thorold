[org 0x7C00]

KERNEL_OFFSET equ 0x1000

call load_kernel

; start Protected Mode
; load in GDT
cli
lgdt [gdt_desc]

mov eax, cr0
or eax, 0x1
mov cr0, eax

jmp 0x8:start_pm

[bits 32]
start_pm:
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	; setup the stack
	mov ebp, 0x9000
	mov esp, ebp
	
	; call kernel
	call KERNEL_OFFSET
	jmp $ ; hang bootloader

[bits 16]
load_kernel:
	; loads kernel into memory
	mov bx, KERNEL_OFFSET
	mov dh, 0xF
	mov dl, 0x0
	
	mov ah, 0x02
	mov al, dh
	mov ch, 0x00
	mov dh, 0x00
	mov cl, 0x02
	int 0x13
	ret
	
; setting up gdt
[bits 16]
gdt:
	; null
	dd 0x0
	dd 0x0
	; code
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 0x9A
	db 0xCF
	db 0x0
	; data
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 0x92
	db 0xCF
	db 0x0
	
gdt_desc:
	dw $ - gdt - 1	; 16 bit address
	dd gdt			; 32 bit address

; zero padding
times 510-($-$$) db 0
dw 0xAA55