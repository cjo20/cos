global _start
global loader
extern kmain


extern kernel_virtual_start;
extern kernel_virtual_end;
extern kernel_physical_start;
extern kernel_physical_end;

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
PAGE_TABLE_ENTRIES	equ 1024
PRIV				equ 0x3

section .data
align 0x1000
BootPageDirectory:
;	; This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
;	; All bits are clear except the following:
;	; bit 7: PS The kernel page is 4MB
;	; bit 1: RW The kernel page is read/write
;	; bit 0: P	The kernel page is present
;	; This entry must be here -- otherwise the kernel will crash immediately after paging is 
;	; enabled because it can't fetch the next instruction. It's ok to unmap this page later
;
;	dd 0x00000083
;	times (KERNEL_PAGE_NUMBER - 1) dd 0 ;Pages before kernel space
;	; This page directory entry defines a 4MB page containing the kernel
;	dd 0x00000083
;	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0 ; Pages after the kernel image
	
	times 1024 dd 0x0
	;times 1024 dd 0x0
Page_0:
	times PAGE_TABLE_ENTRIES dd 0x0
Page_768:
	times PAGE_TABLE_ENTRIES dd 0x0
	


section .text
align 4

loader equ (_start - 0xC0000000)
_start:
	cli

	pusha
	mov eax, (Page_0  - KERNEL_VIRTUAL_BASE)
	mov ebx, 0x0 |  PRIV
	mov ecx, PAGE_TABLE_ENTRIES
.loop:
	mov dword [eax], ebx
	add eax, 4
	add ebx, 4096
	loop .loop

	mov eax, (Page_768 -  KERNEL_VIRTUAL_BASE)
	mov ebx, 0x000000 | PRIV
	mov ecx, PAGE_TABLE_ENTRIES
.loop2:
	mov dword [eax], ebx
	add eax, 4
	add ebx, 4096
	loop .loop2

	mov eax, (Page_0  - KERNEL_VIRTUAL_BASE)
	or eax, 0x3
	mov dword [BootPageDirectory -  KERNEL_VIRTUAL_BASE], eax

	mov eax, (Page_768  - KERNEL_VIRTUAL_BASE)
	or eax, 0x3
	mov dword [BootPageDirectory + (768 * 4) -  KERNEL_VIRTUAL_BASE], eax

	popa

	mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
	mov cr3, ecx ; Load Page Directory Base Register

	mov ecx, cr4
	;or ecx, 0x00000010	; Set PSE bit in CR4 to enable 4MB pages
	mov cr4, ecx

	mov ecx, cr0
	or ecx, 0x80000000 ; Set PG bit in CR0 to enable paging
	mov cr0, ecx

	lea ecx, [StartInHigherHalf]
	jmp ecx

StartInHigherHalf:


;	mov dword [BootPageDirectory], 0
;	invlpg [0]

	mov esp, kernel_stack + KERNEL_STACK_SIZE

	push ebx
	push kernel_physical_end
	push kernel_physical_start
	push kernel_virtual_end
	push kernel_virtual_start
 	call kmain	

 	cli
.hang:
	hlt
	jmp .hang 	

KERNEL_STACK_SIZE	equ 4096

section .bss
align 4
kernel_stack:
	resb KERNEL_STACK_SIZE