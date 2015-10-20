global _start
global loader
extern kmain

MAGIC_NUMBER 	equ 0x1BADB002
MEMALIGN		equ 1<<1
MEMINFO			equ 1<<1
HEADER_FLAGS	equ MEMALIGN | MEMINFO
CHECKSUM	 	equ -(MAGIC_NUMBER + HEADER_FLAGS)


SECTION .__mbHeader
align 0x4
	dd MAGIC_NUMBER
	dd HEADER_FLAGS
	dd CHECKSUM	



KERNEL_VIRTUAL_BASE equ 0xC0000000 ; 3GB
KERNEL_PAGE_NUMBER 	equ (KERNEL_VIRTUAL_BASE >> 22) ; Page directory inext of kernel's 4MB PTE

section .data
align 0x1000
BootPageDirectory:
	; This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
	; All bits are clear except the following:
	; bit 7: PS The kernel page is 4MB
	; bit 1: RW The kernel page is read/write
	; bit 0: P	The kernel page is present
	; This entry must be here -- otherwise the kernel will crash immediately after paging is 
	; enabled because it can't fetch the next instruction. It's ok to unmap this page later

	dd 0x00000083
	times (KERNEL_PAGE_NUMBER - 1) dd 0 ;Pages before kernel space
	; This page directory entry defines a 4MB page containing the kernel
	dd 0x00000083
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0 ; Pages after the kernel image

section .text
align 4

loader equ (_start - 0xC0000000)


_start:
	cli

	mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
	mov cr3, ecx ; Load Page Directory Base Register

	mov ecx, cr4
	or ecx, 0x00000010	; Set PSE bit in CR4 to enable 4MB pages
	mov cr4, ecx

	mov ecx, cr0
	or ecx, 0x80000000 ; Set PG bit in CR0 to enable paging
	mov cr0, ecx

	lea ecx, [StartInHigherHalf]
	jmp ecx

StartInHigherHalf:

	mov dword [BootPageDirectory], 0
	invlpg [0]
	mov esp, kernel_stack + KERNEL_STACK_SIZE
	;xchg bx, bx
	push ebx
	push 3
	push 2
	push 1
	push 0
 	call kmain	
.loop:
	jmp .loop

KERNEL_STACK_SIZE	equ 4096

section .bss
align 4
kernel_stack:
	resb KERNEL_STACK_SIZE