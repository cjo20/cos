global loader

MAGIC_NUMBER 	equ 0x1BADB002
FLAGS		 	equ 0x0
CHECKSUM	 	equ -MAGIC_NUMBER

extern kmain
section .text
align 4
	dd MAGIC_NUMBER
	dd FLAGS
	dd CHECKSUM	


loader:
	mov eax, 0xCAFEBABE
	mov esp, kernel_stack + KERNEL_STACK_SIZE
	;xchg bx, bx
 	call kmain
 	;mov dword [0x000B8000], 0x4128		
.loop:
	jmp .loop

KERNEL_STACK_SIZE	equ 4096

section .bss
align 4
kernel_stack:
	resb KERNEL_STACK_SIZE