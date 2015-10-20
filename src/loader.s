
MAGIC_NUMBER 	equ 0x1BADB002
HEADER_FLAGS	equ 0x00000001
CHECKSUM	 	equ -(MAGIC_NUMBER + HEADER_FLAGS)

SECTION .__mbHeader
align 0x4
	dd MAGIC_NUMBER
	dd HEADER_FLAGS
	dd CHECKSUM	

section .text
align 4
extern kmain
global _start
_start:
	cli
	mov esp, kernel_stack + KERNEL_STACK_SIZE
	;xchg bx, bx
	push ebx
 	call kmain	
.loop:
	jmp .loop

KERNEL_STACK_SIZE	equ 4096

section .bss
align 4
kernel_stack:
	resb KERNEL_STACK_SIZE