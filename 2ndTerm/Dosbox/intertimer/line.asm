.186
.model tiny
.code
locals @@
org 100h

shift equ 146
space equ 4
color equ 5eh

Start:

		xor ax, ax
		mov es, ax
		
		mov bx, 8 * 4
		
		mov ax,   es:[bx]
		mov Old08, ax
		mov ax, es:[bx+2]
		mov Old08+2, ax
		
		cli  ;Clear Interrupt-Enable Flag
		mov ax, cs
		mov word ptr es:[bx], offset New08
		mov word ptr es:[bx + 2], ax
		sti
		mov ah, 31h
		mov dx, offset TheEnd
		shr dx, 4
		inc dx
		int 21h

;------------------------------------------------------------------------------

New08		proc
		push di es ax di es dx bx cx bp ax
		mov di, 0b800h
		mov es, di
		mov di, (5*80 - 9) * 2

		mov ah, color
		mov bp, 7	; len
		mov dl, 13	; num of reg 
		call framedraw
		
		mov di, (6*80 - 8) * 2

		pop ax bp cx bx dx 
		;2) di 1) es - top of the stack

		mov byte ptr es:[di], 'a'
		add di, 2
		mov byte ptr es:[di], 'x'
		add di, space

		call decode
		add di, shift
		
		mov byte ptr es:[di], 'b'
		add di, 2
		mov byte ptr es:[di], 'x'
		add di, space
		
		mov ax, bx
		call decode
		add di, shift

		mov byte ptr es:[di], 'c'
		add di, 2
		mov byte ptr es:[di], 'x'
		add di, space
		
		mov ax, cx
		call decode
		add di, shift


		
		mov byte ptr es:[di], 'd'
		add di, 2
		mov byte ptr es:[di], 'x'
		add di, space
		
		mov ax, dx
		call decode
		add di, 146


		mov byte ptr es:[di], 'b'
		add di, 2
		mov byte ptr es:[di], 'p'
		add di, space
		
		mov ax, bp
		call decode
		add di, shift	
		

		mov byte ptr es:[di], 's'
		add di, 2
		mov byte ptr es:[di], 'p'
		add di, space
		
		mov ax, sp
		add ax, 0ah
		call decode
		add di, shift

		mov byte ptr es:[di], 's'
		add di, 2
		mov byte ptr es:[di], 'i'
		add di, space
		
		mov ax, si
		call decode
		add di, shift

		

		mov byte ptr es:[di], 'd'
		add di, 2
		mov byte ptr es:[di], 's'
		add di, space
		
		mov ax, ds
		call decode
		add di, shift

		

		mov byte ptr es:[di], 'c'
		add di, 2
		mov byte ptr es:[di], 's'
		add di, space
		
		mov ax, cs
		call decode
		add di, shift
			


		mov byte ptr es:[di], 's'
		add di, 2
		mov byte ptr es:[di], 's'
		add di, space
		
		mov ax, ss
		call decode
		add di, shift


		pop es;
	 	;di - top of the stack

		push di
		mov ax, es
		mov di, 0b800h
		mov es, di
		pop di
		
		
		mov byte ptr es:[di], 'e'
		add di, 2
		mov byte ptr es:[di], 's'
		add di, space
		
		
		call decode
		add di, shift



		mov ax, di
		pop  di	
		xchg ax, di
		
		
		mov byte ptr es:[di], 'd'
		add di, 2
		mov byte ptr es:[di], 'i'
		add di, space
		
		
		call decode
		

		pop ax es di
		
		db 0eah
Old08		dw 0
		dw 0
	
		endp

;------------------------------------------------------------------------------
		
;------------------------------------------------------------------------------
;Expects: ax - register to decode
;	  es:[di] - where to write
;
;Destroy: ax
;------------------------------------------------------------------------------
Decode		proc
		
		push  cx ax
		mov cx, 3
		
		shr ax, 2
		and ah, 0011b

@@start:	cmp ah, 0ah
		jnb @@Letter1

		add ah, 30h
		jmp @@end1

@@Letter1:	add ah, 41h
		sub ah, 0ah

@@end1:		mov byte ptr es:[di], ah
		add di, 2

		shl ax, 2
		and ah, 0011b

		loop @@start

		pop ax
		and al, 011b

		cmp al, 0ah
		jnb @@Letter2

		add al, 30h
		jmp @@end2

@@Letter2:	add al, 41h
		sub al, 0ah

@@end2:		mov byte ptr es:[di], al
		add di, 2
		
		pop cx

		ret
		endp

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;Expects: 	
;		bp - length of main part
;		dl - height
;		es:[di] - video memory
;
;Destroy:	al, bh, dx, cx
;
;------------------------------------------------------------------------------
framedraw	proc
		
		mov cx, bp
		
		mov al, 201	; top
		mov bh, 205
		mov dh, 187
		call lineb

Drawl:		mov al, 186;	the main part
		mov bh, 32
		mov dh, 186
		call lineb
	
		dec dx
		cmp dl, 0
		jne drawl

		mov al, 200	; bottom
		mov bh, 205
		mov dh, 188
		call lineb
		ret
		endp

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;Expects: 	al - first element of the line ascii 
;		bh - main part of the line ascii
;		dh - last element of the line ascii
;		bp = cx - length of main part
;		es:[di] - video memory
;		
;
;Destroy:	al (equal to dh)
;
;		Draw the line
;------------------------------------------------------------------------------

LineB		proc
	
		stosw
		
		mov al, bh
		rep stosw
		mov cx, bp

		mov al, dh
		stosw
		
		add di, 156
		sub di, bp
		sub di, bp 
		ret
		endp

;---------------------------------------------------------------------------

theEnd:
end		Start