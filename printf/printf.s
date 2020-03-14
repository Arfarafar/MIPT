section .text

global _start

;-----------------------------------------------------
; expects: end of stack is the ascii to print
;			cx - length of ascii bytes to print
;
; all functions which use this macro act like this:
; push bx with ascii code in bh to form (bl bh) on the stack's top
; inc sp to erase bl, inc cx, 
; so you need to form your string from the end 
;
; destroy:  rcx, rax, rdx, rdi, rsi
;-----------------------------------------------------
%macro  PUTNUMFROMSTACK 0
;          push rax
 ;         push rcx
;          push rdi
;          push rsi
;          push rdx   
		
		 
	  mov rdx, rcx    ; strlen (str)  
			
          mov rax, 0x01      
          mov rdi, 1         ; stdout

          mov rsi, rsp			; where str starts
	  
          add rsp, rcx			; pop out the str bytes from stack
	  				; return sp to it's normal state
          
          syscall

 ;         pop rcx
;          pop rax
;          pop rdi
;          pop rsi
;          pop rdx

%endmacro
;-----------------------------------------------------------


; for useage with signed numbers but never used
;-----------------------------------------------------------
%macro TranslateFromDopcode 0			
	    cmp ax, 0
            jg  .numberhandling
            not ax	;перевод из дополнительного кода в обычный
            inc ax
%endmacro
;------------------------------------------------------------



;------------------------------------------------------------
;expects: ah - ascii code of symbol to print 
;
;destroy: ax, rsi, rdi
;
;------------------------------------------------------------
charascii: 	push rcx
			push rdx

			push ax
			inc sp
			mov rcx , 1


            PUTNUMFROMSTACK
            pop rdx
            pop rcx
            ret
;------------------------------------------------------------



;------------------------------------------------------------
;expects:  rax - offset of str
;
; Return: rdx - strlen
;------------------------------------------------------------

strlen		push rax
			xor rdx, rdx

.nextchar:	cmp byte [rax], '$'
			je .end
			inc rdx
			inc rax
			jmp .nextchar


.end:		pop rax
			ret
;-------------------------------------------------------------





;------------------------------------------------------------
;	expects: rах - offset str to print 
;
;	destroy: rax, rsi, rdi
;
; Print the string located in [rax] on the screen
;------------------------------------------------------------
string:		push rcx
			push rdx
			
			call strlen ; returns strlen in rdx 

			mov rsi, rax    ; offset 			
          	mov rax, 0x01      
         	mov rdi, 1         ; stdout
        		
          
          	syscall
          	pop rdx
            pop rcx
            ret
;------------------------------------------------------------



;------------------------------------------------------------
;expects: ax - num to print dec
;
; destroy: ax, bx, rsi, rdi
;
;------------------------------------------------------------
dec:       	push rdx
			push rcx
			
			xor rcx, rcx
     		xor dx, dx

            
.numberhandling:
			mov bx, 10d
			div bx
			mov bx, dx
			add bx, '0'

			shl bx, 8 ;  mov to bh
			inc cx 
			push bx
			inc sp   ;erase 00 byte left from bl

			xor dx, dx
			cmp ax, 0
			jne .numberhandling

            PUTNUMFROMSTACK


.end:		
			pop rcx
			pop rdx
			ret
;------------------------------------------------------------






;-------------------------------------------------------------
;expects: ax - num to print bin
;
; destroy: ax, bx, rsi, rdi
;
;------------------------------------------------------------
bin:        push rdx
			push rcx

			xor rcx, rcx
			xor bx, bx
            
            
.numberhandling:
			
			shr ax, 1
			jc .mov1		; if there was 1 in last bit
			mov bh, '0'
			jmp .leap
.mov1:		mov bh, '1'
.leap:		inc cx
			push bx
			inc sp  ;erase 00 byte left from bl
			cmp ax, 0
			jne .numberhandling

            PUTNUMFROMSTACK


.end:		pop rcx
			pop rdx
			ret
            
            
;------------------------------------------------------------

;-------------------------------------------------------------
;expects: ax - num to print oct
;
; destroy: ax, bx, rsi, rdi
;------------------------------------------------------------
oct:		push rdx
			push rcx
			
			xor rcx, rcx
			xor bx, bx
            
            
.numberhandling:
			mov bx, ax
			shr ax, 3
			and bx, 111b
			
			add bx, '0'
			shl bx, 8 ;  mov to bh
			inc cx 
			push bx
			inc sp   ;erase 00 byte left from bl

			cmp ax, 0
			jne .numberhandling

            PUTNUMFROMSTACK


.end:		pop rcx
			pop rdx
			ret


;-------------------------------------------------------------
;expects: ax - num to print hex
;
; destroy: ax, bx, rsi, rdi
;------------------------------------------------------------
hex:        
			push rdx
			push rcx
			
			xor rcx, rcx
			xor bx, bx
            
            
.numberhandling:
			mov bx, ax
			shr ax, 4
			and bx, 1111b
			cmp bx, 9
			ja .notanumber	
			add bx, '0'
			jmp .endhandling
.notanumber:
			add bx, 'A'	
			sub bx , 10
.endhandling:
			shl bx, 8 ;  mov to bh
			inc cx 
			push bx
			inc sp   ;erase 00 byte left from bl

			cmp ax, 0
			jne .numberhandling

            PUTNUMFROMSTACK


.end:		pop rcx
			pop rdx
			ret
     
;-------------------------------------------------------------

;-------------------------------------------------------------
;получает в стеке форматную строку и аргументы
;печатает ее
;
;--------------------------------------------------------------
printf:		pop rcx ; адрес возврата
			pop rdx ; offset форматная строка 

.nextchar:	cmp byte [rdx], '$'
			je .end

			cmp byte [rdx], '%'
			je .specsymbol

			mov ah, [rdx]
			call charascii
			inc rdx
			jmp .nextchar

.specsymbol:
			inc rdx

			cmp byte [rdx], 's' ;string handler requires addres in [rax]
			jne .percent
			pop rax
			call string
			jmp .endspec

.percent:	cmp byte [rdx], '%'
			jne .hex
			mov ah, '%'
			call charascii 
			jmp .endspec

.hex:		cmp byte [rdx], 'x' ;hex handler requires num in rax
			jne .oct
			pop rax
			call hex 
			jmp .endspec

.oct:		cmp byte [rdx], 'q' ;oct handler requires num in rax
			jne .bin
			pop rax
			call oct
			jmp .endspec

.bin:		cmp byte [rdx], 'b' ;bin handler requires num in rax
			jne .dec
			pop rax
			call bin
			jmp .endspec 

.dec:		cmp byte [rdx], 'd' ;dec handler requires num in rax
			jne .char
			pop rax, 
			call dec
			jmp .endspec 

.char:		cmp byte [rdx], 'c' ;char handler requires ascii in rax
			jne .end
			pop rax
			shl ax, 8
			call charascii 


.endspec:	inc rdx
			jmp .nextchar

.end:		push rcx
			ret
;------------------------------------------------------------ 

;------------------------------------------------------------

_start:   
			push 1300d
			push 100d

			push 0edah
			push Firstarg
			push 100b
			push 'I'
			push Teststring
			call printf

            mov rax, 0x3C      ; exit64 (rdi)
            xor rdi, rdi
            syscall
;------------------------------------------------------------

section .data

Teststring: db "%c absolutely, %b, %s %x, %d%%! %q", 13, 10 ,'$'
Firstarg: db "Love", '$'
;Second arg: db ""
