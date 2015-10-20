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
	ret

