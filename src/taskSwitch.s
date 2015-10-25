section .text
global switchTask
switchTask:
	pusha
	pushf
	mov eax, cr3
	push eax
	mov eax, [esp + 44] ; the first argument
	mov [eax + 4], ebx
	mov [eax + 8], ecx
	mov [eax + 12], edx
	mov [eax + 16], esi
	mov [eax + 20], edi
	mov ebx, [esp + 36] ; EAX
	mov ecx, [esp + 40] ; IP
	mov edx, [esp + 20] ; ESP
	add edx, 4 ; Remove the return value
	mov esi, [esp + 16] ; EBP
	mov edi, [esp + 4] ; EFLAGS
	mov [eax], ebx
	mov [eax + 24], edx
	mov [eax + 28], esi
	mov [eax + 32], ecx
	mov [eax + 36], edi
	pop ebx
	mov [eax + 40], ebx
	push ebx

	mov eax, [esp + 48]
	mov ebx, [eax + 4]
	mov ecx, [eax + 8]
	mov edx, [eax + 12]
	mov esi, [eax + 16]
	mov edi, [eax + 20]
	mov ebp, [eax + 28]
	push eax
	mov eax, [eax + 36]
	push eax
	popf
	pop eax
	mov esp, [eax + 24]
	push eax
	mov eax, [eax + 40]
	mov cr3, eax
	pop eax
	push eax
	mov eax, [eax + 32]
	xchg eax, [esp]
	mov eax, [eax + 4]
	ret


