 .model tiny
 .code
locals @@
org 100h
  
Start:
			     mov ax, ds
     			     mov es, ax
  
  
    			     mov al, 90h	      ;	value to be found
     			     lea di, src
     			    
    
     			     call strchr
  
     			     mov ax, 4c00h
     			     int 21h

include func.asm

     
src:	   	db    "Hello world!", 13, 10, 90h, "$"

dst: 		db "0"
     
end	     Start





















