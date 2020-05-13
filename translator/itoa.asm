section .text

global _start

;-----------------------------------------------------
; expects: end of stack is the ascii to print
;                 cx - length of ascii bytes to print
;
; destroy:  rcx, rax, rdx, rdi, rsi
;-----------------------------------------------------
%macro  PUTNUMFROMSTACK 0

             
              mov rdx, rcx    ; strlen (str)  
                  
          mov rax, 0x01      
          mov rdi, 1         ; stdout

          mov rsi, rsp
          add rsp, rcx              ; pop out the number from stack
          
          syscall

%endmacro
;-----------------------------------------------------------



;------------------------------------------------------------
;expects: ax - num to print dec
;
; destroy: ax, bx, rsi, rdi
;
;------------------------------------------------------------
printf:           
                  xor rcx, rcx
                  xor edx, edx
                  xor si, si
                  cmp eax, 0
                  jg  .drob
                  not eax      
                  inc eax
                  inc si

.drob:            mov ebx, 1000
                  imul rbx
                  shr rax, 10   
.numberhandling:
                  xor edx, edx
                  mov ebx, 10d
                  div ebx
                  mov ebx, edx
                  add ebx, '0'

                  shl bx, 8 ;  mov to bh
                  inc ecx 
                  push bx
                  inc rsp   ;erase 00 byte left from bl

                  
                  cmp ecx, 3
                  je .dot
                  cmp eax, 0
                  jne .numberhandling
                  jmp .minus

.dot:             xor ebx, ebx
                  add ebx, '.'
                  shl ebx, 8 ;  mov to bh
                  inc ecx 
                  push bx
                  inc rsp 
                  jmp .numberhandling

.minus:     cmp si, 1
            jne .pri
            xor ebx, ebx
            add ebx, '-'
            shl ebx, 8 ;  mov to bh
            inc ecx 
            push bx
            inc rsp  

.pri:      xor ebx, ebx
                  add ebx, 13
                  shl ebx, 8 ;  mov to bh
                  inc ecx 
                  push bx
                  inc rsp  
            xor ebx, ebx
                  add ebx, 10
                  shl ebx, 8 ;  mov to bh
                  inc ecx 
                  push bx
                  inc rsp          
         PUTNUMFROMSTACK


.end:       
                 
                  ret
;------------------------------------------------------------









_start:     

            