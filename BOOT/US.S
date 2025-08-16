	org	0

	dc.b	'SEGA SEGASATURN '
	dc.b	'SEGA TP T-132   '
	dc.b	'T-13205H  V1.000'
	dc.b	'19961008CD-1/1  '
	dc.b	'U               '
	dc.b	'EATJ            '
	dc.b	'POWER SLAVE     '
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
	incbin	sys_areu.bin		; teritory codes

	mov.l	=$06004000,r0
	jmp	@r0
	nop

	littab

	cnop	0,4

ipsize



