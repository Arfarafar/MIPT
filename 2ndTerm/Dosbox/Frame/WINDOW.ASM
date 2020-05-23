.model tiny
.code
org 100h

Start:		mov di, 0b800h 
		mov es, di
		
		

		mov bp, 25; 	width	
		mov si, 7; 	height
		mov bl, 3; 	rep

beg:		mov ah,  30h

		mov cx, bp
		mov dx, si
				
		mov di, ((5*40) + 27) shl 1
		

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

		mov ax, 5000h	; horizontal shadow
 		add di, 6
		add cx, 3
		rep stosw
		
		mov dx, si
		inc dx
drawSh:		sub di, 168	;shadow vertical
		mov cx, 4
		rep stosw 
		dec dx
		cmp dx, 0
		jne drawSh
		
		cmp bl, 0
		je framend
		dec bl
		add si, 2
		add bp, 2
		jmp beg
		
		

framend:	mov ax, 4c00h
		int 21h

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;Expects: 	al - first element of the line ascii 
;		bh - main part of the line ascii
;		dh - last element of the line ascii
;		cx - length of main part
;		es:[di] - video memory
;
;Destroy:	al (equal to dh)
;
;		Draw the line
;------------------------------------------------------------------------------

LineB:		
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

;------------------------------------------------------------------------------				

end		Start









