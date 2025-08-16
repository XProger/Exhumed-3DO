	org	0

	dc.b	'SEGA SEGASATURN '
	dc.b	'SEGA TP T-180   '
	dc.b	'T-18001G  V1.003'
	dc.b	'19961002CD-1/1  '
	dc.b	'J               '
	dc.b	'JA              '
	dc.b	'1999            '
	dc.b	'                '
	dc.b	'                '
	dc.b	'                '
	dc.b	'                '
	dc.b	'                '
	dc.b	'                '
	dc.l	0,0,0,0
	dc.l	ipsize,0,0,0
	dc.l	$06004000,0,0,0		; first value is address to load 1st file on cd

	incbin	sec1.bin		; sega logo
	incbin	sys_arej.bin		; teritory codes

	mov.l	=$06004000,r0
	jmp	@r0
	nop

	littab

	cnop	0,4

ipsize



