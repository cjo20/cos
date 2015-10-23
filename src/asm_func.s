section .text
align 4

global flush_gdt

;flush_gdt - Writes GDT
;stack: [esp + 4] Pointer to gdt
;		[esp	] The return address
flush_gdt:

	mov eax, [esp + 4]
	lgdt 	[eax]	; read a byte from the I/O port and store it in the al register
	jmp 0x08:flush2
flush2:
	mov ax, 0x10
	mov dx, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret 				; return


global io_wait

io_wait:
	out 0x80, al
	ret

; get_cpuid
; stack: 	[esp + 8] Pointer to int[4]
;			[esp + 4] Argument to cpuid (0 or 1)
;			[esp	] Return Address
global get_cpuid
get_cpuid:
	mov eax, [esp + 4]
	cpuid
	mov edi, [esp + 8]
	mov [edi], eax
	add edi, 4
	mov [edi], ebx
	add edi, 4
	mov [edi], ecx
	add edi, 4
	mov [edi], edx
	ret


; pmmngr_load_PDBR:
; Loads page table directory
; stack: 	[esp + 4] pointer to page directory
;			[esp	] return address

global pmmngr_load_PDBR
pmmngr_load_PDBR:
	mov eax, [esp + 4]
	mov cr3, eax
	ret


; pmmngr_load_PDBR:
; Loads page table directory
; stack: 	[esp + 4] pointer to page directory
;			[esp	] return address

global pmmngr_get_PDBR
pmmngr_get_PDBR:
	mov eax, cr3
	ret

global flush_TLB
flush_TLB:
	mov eax, [esp + 4]
	invlpg [eax]
	ret

global pmmngr_paging_enable
pmmngr_paging_enable:
	mov eax, cr0
	cmp dword [esp + 4], 1
	je enable
	jne disable
enable:
	or eax, 0x80000000
	mov cr0, eax
	jmp done
disable:
	and eax, 0x7FFFFFFF
	mov cr0, eax
done:
	ret