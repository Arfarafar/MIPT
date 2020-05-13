.section	.text._Z4rollji,"axG",@progbits,_Z4rollji,comdat
	.weak	_Z4rollji
	.type	_Z4rollji, @function
_Z4rollji:
.LFB35:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	$0, -8(%rbp)
.L88:
	movl	-8(%rbp), %eax
	cmpl	-24(%rbp), %eax
	jge	.L87
	movl	-20(%rbp), %eax
	shrl	$31, %eax
	movl	%eax, -4(%rbp)
	movl	-20(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -20(%rbp)
	addl	$1, -8(%rbp)
	jmp	.L88
.L87:
	movl	-20(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE35:
	.size	_Z4rollji, .-_Z4rollji
	.text
	.globl	_Z5RecurPKci
	.type	_Z5RecurPKci, @function
_Z5RecurPKci:
.LFB36:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	cmpl	$0, -12(%rbp)
	jne	.L91
	movl	$0, %eax
	jmp	.L92
.L91:
	movl	-12(%rbp), %eax
	leal	-1(%rax), %edx
	movq	-8(%rbp), %rax
	addq	$1, %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	_Z5RecurPKci
	movl	$1, %esi
	movl	%eax, %edi
	call	_Z4rollji
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	xorl	%edx, %eax
.L92:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc