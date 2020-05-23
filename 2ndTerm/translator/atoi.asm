section .text

global _start

;-----------------------------------------------------
; expects: rsi - буфер на 32 
; rcx - адрес переменной
;
; destroy:  rcx, rax, rdx, rdi, rsi
;-----------------------------------------------------
scanf:  
                xor rdx, rdx
                xor di, di
                xor eax, eax
                mov ebx, 10

                cmp byte [rsi], '-'
                jne .plus
                inc rsi
                inc di


.plus:          cmp byte [rsi], ';'
                je .loopend
                mul ebx
                mov dl, byte [rsi]
                add eax, edx
                xor dl, dl
                sub eax, 48
                inc rsi
                jmp .plus
                
                

.loopend:       cmp di, 1
                jne .end
                dec eax
                not eax

.end:           shl eax, 10
                mov [rcx], eax
                ret

;------------------------------------------------------------





;------------------------------------------------------------









_start:     

            