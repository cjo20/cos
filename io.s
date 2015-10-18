global outb

;outb - send a byte to an I/O port
;stack: [esp+8] the data byte
;		[esp+4] the I/O port
;		[esp]	return address

outb:
	mov al, [esp + 8]
	mov dx, [esp + 4]
	out dx, al
	ret

global inb

;inb - returns a byte from a given I/O port
;stack: [esp + 4] The address of the I/O port
;		[esp	] The return address
inb:
	mov dx, [esp + 4] 	;move the adress of the I/O port to the dx register
	in 	al, dx 		  	; read a byte from the I/O port and store it in the al register
	ret 				; return the read byte