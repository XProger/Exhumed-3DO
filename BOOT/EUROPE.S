	org	0

	dc.b	'SEGA SEGASATURN '
	dc.b	'SEGA ENTERPRISES'
	dc.b	'MK-81084  V1.000'
	dc.b	'19960726CD-1/1  '
	dc.b	'E               '
	dc.b	'J               '
	dc.b	'EXHUMED         '
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
	incbin	sys_aree.bin		; teritory codes

	mov.l	=$06004000,r0
	jmp	@r0
	nop

	littab

	cnop	0,4

ipsize



