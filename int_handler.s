section .text
align 4
%macro no_error_code_interrupt_handler 1
global isr%1
isr%1:
	push 	dword 0
	push 	dword %1
	jmp 	common_interrupt_handler
%endmacro

%macro error_code_interrupt_handler 1
global isr%1
isr%1:
	push 	dword %1
	jmp 	common_interrupt_handler
%endmacro

%macro no_error_code_irq_handler 1
global irq%1
irq%1:
	push dword 0
	push dword %1
	jmp irq_common_stub
%endmacro




extern irq_handler
irq_common_stub:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, irq_handler
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret

extern interrupt_handler
common_interrupt_handler:
	ousha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, interrupt_handler
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret

no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
no_error_code_interrupt_handler 9
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
no_error_code_interrupt_handler 21
no_error_code_interrupt_handler 22
no_error_code_interrupt_handler 23
no_error_code_interrupt_handler 24
no_error_code_interrupt_handler 25
no_error_code_interrupt_handler 26
no_error_code_interrupt_handler 27
no_error_code_interrupt_handler 28
no_error_code_interrupt_handler 29
no_error_code_interrupt_handler 30
no_error_code_interrupt_handler 31


error_code_interrupt_handler 8
error_code_interrupt_handler 10
error_code_interrupt_handler 11
error_code_interrupt_handler 12
error_code_interrupt_handler 13
error_code_interrupt_handler 14
error_code_interrupt_handler 17

no_error_code_irq_handler 0
no_error_code_irq_handler 1
no_error_code_irq_handler 2
no_error_code_irq_handler 3
no_error_code_irq_handler 4
no_error_code_irq_handler 5
no_error_code_irq_handler 6
no_error_code_irq_handler 7
no_error_code_irq_handler 8
no_error_code_irq_handler 9
no_error_code_irq_handler 10
no_error_code_irq_handler 11
no_error_code_irq_handler 12
no_error_code_irq_handler 13
no_error_code_irq_handler 14
no_error_code_irq_handler 15



global load_idt

; load_idt - Loads the interrupt descriptor table (IDT)
; stack: 	[esp + 4] the address of the first entry in the IDT
;			[esp	] the return address
load_idt:
	mov 	eax, [esp + 4]
	lidt 	[eax]
	ret