;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;Expects:		al - value to find
;			es:[di] - memory to search with
;
;returns:		si = 0 if value not found
;			else es:[si] - value
;
;Deconstruct:		cx
;------------------------------------------------------------------------------
    
strchr	    		proc
			mov si, di
			cld
@@nextchar:		cmp byte ptr es:[di], "$"
			je @@failure
    			scasb
			je @@success
			jmp @@nextchar
    			     
@@success:  		dec di
	  		xchg si, di
			jmp @@end

@@failure:		mov di, si
			xor si, si

@@end:	     		ret
    			endp


;---------------------------------------------------------------------------

;------------------------------------------------------------------------------
;Expects:		es:[di] - string 
;			
;returns:		cx - len of string
;------------------------------------------------------------------------------

strlen	    		proc
			xor cx, cx
			
@@nextchar:		cmp byte ptr es:[di], "$"
			je @@end
    			inc cx
			inc di
			jmp @@nextchar
    			     
@@end:	     		sub di, cx
			ret
    			endp

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;Expects:		al - value to find
;			es:[di] - string where to find
;			
;
;returns:		si = 0 if value not found
;			else es:[si] - value
;
;Deconstruct:		cx
;------------------------------------------------------------------------------

strrchr	    		proc
			mov si, di
			call strlen	
			add di, cx
			std
			
			repne scasb
     			inc di
     			jne @@failure

     			xchg si, di
			jmp @@end

@@failure:		mov di, si
			
@@end:	     		cld

			ret
    			endp

;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
;Expects:		es:[di] - str1
;			ds:[si] - str2
;
;returns:		nothing
;
;Deconstruct:		cx, bx
;
;			copy str2 to str1
;------------------------------------------------------------------------------
strcpy			proc
			xor cx, cx
			
@@nextchar:		cmp byte ptr ds:[si], "$"
			je @@end
    			inc cx
			inc si
			jmp @@nextchar
    			     
@@end:	     		sub si, cx	

			inc cx
			mov bx, cx
			cld
			rep movsb
			sub di, bx
			sub si, bx
			ret 
			endp

;------------------------------------------------------------------------------



;------------------------------------------------------------------------------
;Expects:		es:[di] - str1
;			ds:[si] - str2
;			cx - bytes to copy from srt2 to str1
;
;returns:		nothing
;
;Deconstruct:		cx, bx
;------------------------------------------------------------------------------
memcpy			proc

			mov bx, cx
			cld
			rep movsb
			sub di, bx
			sub si, bx
			ret 
			endp

;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
;Expects:		al - value to find
;			es:[di] - memory to search with
;			cx - num of bytes to search
;
;returns:		si = 0 if value not found
;			else es:[si] - value
;
;Deconstruct:		cx
;------------------------------------------------------------------------------
memchr	    		proc
			mov si, di
			cld
    			repne scasb
     			dec di
     			jne @@failure

     			xchg si, di
			jmp @@end

@@failure:		xor si,si

@@end:	     		
			ret
    			endp
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;Expects:		cx - num of bytes
;			es:[di] - memory to set
;
;returns:		nothing
;
;Deconstruct:		cx, di
;------------------------------------------------------------------------------

memset			proc
			rep stosw
			ret
			endp
;------------------------------------------------------------------------------



;------------------------------------------------------------------------------
;Expects:		es:[di] - str2
;			ds:[si] - str1
;
;returns:		bx = 1  if str1 > str2
;			bx = -1 if str1 < str2
;			bx = 0 if str1 == str2
;
;Deconstruct:		cx
;------------------------------------------------------------------------------

memcmp			proc

			mov bx, cx
			cld
			repe cmpsb
			
			sub di, bx 
			sub si, bx 
			add di, cx
			add si, cx


			jb @@firstlow
			ja @@firstgreat
			xor bx,bx
			jmp @@end

@@firstlow:		mov bx, -1
			jmp @@end

@@firstgreat:		mov bx, 1

@@end:			ret 
			endp
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;Expects:		es:[di] - str2
;			ds:[si] - str1
;
;returns:		bx = 0 if str1 doesnt contain str2
;			ds:[bx] - first occurrence str2 in str1
;
;Deconstruct:		ax
;------------------------------------------------------------------------------

strstr			proc
			mov bx, si  
			

			xor cx, cx
@@nextchar:		cmp byte ptr es:[di], "$"
			je @@endline
    			inc cx
			inc di
			jmp @@nextchar
    			     
@@endline:     		sub di, cx
			
			mov ax, cx
			cld

@@cmp:			repe cmpsb

			sub di, ax 
			add di, cx

			je @@success

			
			add si, cx
			cmp byte ptr ds:[si], "$"
			je @@failure
			
			sub si, ax
			inc si
			mov cx, ax
			jmp @@cmp

@@success:  		sub si, ax 
			add si, cx
	  		xchg si, bx
			jmp @@end

@@failure:		mov si, bx
			xor bx, bx

@@end:	     		ret
    			endp


;------------------------------------------------------------------------------
