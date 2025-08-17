	xref __SN_ENTRY_POINT
	xdef __MYENTRYPOINT

	section firstOne
__MYENTRYPOINT
	.align 4

;	mov.l =(1024*512/4),r0 ;move 1/2 meg
;	mov.l =$06004000,r1 ;source
;	mov.l =$00200000,r2 ;dest

;@1	mov.l @r1+,r3
;	dt r0
;	mov.l r3,@r2
;	bf/s @1
;	add.l #4,r2

	mov.l	=__SN_ENTRY_POINT,r0
	jmp	@r0
	nop

	.align 4

	littab

	cnop	0,4
