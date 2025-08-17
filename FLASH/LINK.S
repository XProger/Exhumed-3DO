	xdef executeLink

	section .text

	.align 4

; void executeLink(void *data,int sizeInDWords)
executeLink:
	mov =$06004000,r1

	; copy data to proper position
@1	mov.l @r4+,r0
	mov.l r0,@r1
	add #4,r1
	dt r5
	bf @1

	; setup stack register
	mov.l =$06001000,r15
	; jump to start
	mov.l =$06004000,r1
	jmp @r1
	nop

	.align 4
	littab