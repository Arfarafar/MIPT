.globl	_Z5RecurPKci
	.type	_Z5RecurPKci, @function
_Z5RecurPKci:
.LFB105:
	.cfi_startproc
	
	testl	%esi, %esi
    jle	.L4

	leaq	-1(%rdi, %rsi), %rdx #char* passed =  (key + len - 1 );
	
	xorl	%eax, %eax  #unsigned int result = 0;

	
.L3:
	movzbl	(%rdx), %ecx #char whattoroll = *passed;
	subq	$1, %rdx        # passed--;
	addl	%ecx, %eax  #result += whattoroll;
	roll	%eax
	xorl	%ecx, %eax

	cmpq	%rdx, %rdi
	jna	.L3
	ret
	
.L4:
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE105:
	.size	_Z5RecurPKci, .-_Z5RecurPKci