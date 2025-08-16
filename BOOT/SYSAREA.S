	org	0

	dc.b	'SEGA SEGASATURN '
	dc.b	'SEGA TP T-1299  '
	dc.b	'T-12991G  V1.000'
	dc.b	'19950418CD-1/1  '
	dc.b	'JTUBKAEL        '
	dc.b	'J               '
	dc.b	'POWER SLAVE     '
	dc.b	'                '
	dc.b	'                '
	dc.b	'                '
	dc.b	'                '
	dc.b	'                '
	dc.b	'                '
	dc.l	0,0,0,0
	dc.l	ipsize,0,0,0
	dc.l	$06010000,0,0,0		; first value is address to load 1st file on cd

	incbin	sec1.bin		; sega logo
	incbin	sec2.bin		; teritory codes

	mov.l	=$06010000,r0
	jmp	@r0
	nop

	littab

	cnop	0,4

ipsize



