.globl _Z13Find_By_ValueP4ListPc
.type _Z13Find_By_ValueP4ListPc, @function
_Z13Find_By_ValueP4ListPc:
	.cfi_startproc
	
	pushq	%r14
	pushq	%r13
	pushq	%r11
	pushq 	%rbx
	
	movq	(%rdi), %r14	#cell*
	xor 	%r13, %r13	#counter
	movl	20(%rdi), %r11d #size
	movl	8(%rdi), %ebx	# curr = head
	
.L56:
	cmpl	%r11d, %r13d	
	jge	.L69
	
	salq	$4, %rbx		
	addq	%r14, %rbx	#rbx = (cell+cur)	
		
	movq	(%rbx), %rdi #elem_t val
	call	strcmp@PLT
	testl	%eax, %eax
	addl	$1, %r13d	#count ++
	je	.L55
	

                	#rbx = (cell+cur)
	movl	8(%rbx), %ebx #ebx = next
	jmp	.L56
.L55:
	cmpl	%r11d, %r13d
	jne	.L57
.L69:
	movl	$0, %eax
	jmp	.L58
.L57:
	movl	$1, %eax
.L58:
	
	popq	%rbx
	popq	%r11
	popq	%r13
	popq	%r14
.L85:	
	ret
	.cfi_endproc
