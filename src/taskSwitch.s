section .text


;global switchTask
;switchTask:
;	pusha
;	pushf
;	mov eax, cr3
;	push eax
;	mov eax, [esp + 44] ; the first argument
;	mov [eax + 4], ebx
;	mov [eax + 8], ecx
;	mov [eax + 12], edx
;	mov [eax + 16], esi
;	mov [eax + 20], edi
;	mov ebx, [esp + 36] ; EAX
;	mov ecx, [esp + 40] ; IP
;	mov edx, [esp + 20] ; ESP
;	add edx, 4 ; Remove the return value
;	mov esi, [esp + 16] ; EBP
;	mov edi, [esp + 4] ; EFLAGS
;	mov [eax], ebx
;	mov [eax + 24], edx
;	mov [eax + 28], esi
;	mov [eax + 32], ecx
;	mov [eax + 36], edi
;	pop ebx
;	mov [eax + 40], ebx
;	push ebx
;
;	mov eax, [esp + 48]
;	mov ebx, [eax + 4] ;EBX
;	mov ecx, [eax + 8] ;ECX
;	mov edx, [eax + 12] ;EDX
;	mov esi, [eax + 16] ;ESI
;	mov edi, [eax + 20] ;EDI
;	mov ebp, [eax + 28] ;EBP
;	push eax
;	mov eax, [eax + 36] ;EFLAGS
;	push eax
;	popf
;	pop eax
;	mov esp, [eax + 24] ;ESP
;	push eax
;	mov eax, [eax + 40] ;CR3
;	mov cr3, eax
;;	xchg bx, bx
;	pop eax
;	push eax
;	mov eax, [eax + 32] ;EIP
;	xchg eax, [esp]
;	mov eax, [eax] ;EAX
;
;	ret

;global launch_task
;launch_task:
;	mov eax, 0x20
;	out 0x20, eax
;	iret

;	[esp	] Return Address
;	[esp + 4] New task pointer
;	[esp + 8] Old task pointer
global custom_switch
custom_switch:
;	xchg bx, bx
	push  eax
	mov eax, [esp + 8] 		; Now eax has been backed up, use eax to index old struct
	mov [eax + 4], ebx
	pop ebx
	mov [eax], ebx
	mov [eax + 8], ecx
	mov [eax + 12], edx
	mov [eax + 16], esi
	mov [eax + 20], edi
	mov [eax + 24], esp
	mov [eax + 28], ebp
	mov ecx, [esp]
	mov [eax + 32], ecx 	; esp should still be pointing to return address of this function
	mov [esp - 8], ecx		; make space for eflags and cs


	add dword [eax + 24], -8	; add 2 more values to old stack
	add esp, -8		  		; and add them to the current stack as well

	pushf
	pop ecx 				; ecx now contains eflags
	mov [eax + 36], ecx		; store eflags in the struct

	mov edx, [eax + 24]		; point edx to the stack
	mov ebx, [edx]			; old stack head contains return address

	mov [edx + 8], ecx		; store eflags
	mov dword [edx + 4], 0x8; store code segment

	mov ecx, cr3
	mov [eax + 40], ecx
	; Old task struct should now be full. Move on to restoring values from new task

;	xchg bx, bx
	mov eax, [esp + 16]
	mov ebx, [eax + 40] ; cr3 early
	mov cr3, ebx
	mov ebx, [eax + 4]		; ebx
	mov ecx, [eax + 8]		; ecx
	mov edx, [eax + 12]		; edx
	mov esi, [eax + 16]		; esi
	mov edi, [eax + 20]		; edi
	mov esp, [eax + 24]		; esp
	mov ebp, [eax + 28]		; ebp
	; EIP already head of stack
	; eflags already on stack too
	; cr3 dealt with. That's all of the registers.
	iret
	
