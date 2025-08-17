	xdef rectTransform
	xdef normTransform
	xdef project_point

	section .text

	.align 4
	littab

; void project_point(MthXyz *v,XyInt *p);

	.align 4
project_point:
	mov.l =$0ffffff00,r0
	ldc r0,gbr ; point gbr to hardware divider
	mov.l @(r4,8),r0
	mov.l #1,r1
	shll16 r1
	cmp/gt r1,r0
	bt @1
	mov.l r1,r0
@1      ; ... r0 now holds the z coord or F(1), whichever is greater
	; ... start the hardware divide
	mov.l r0,@(gbr,0)
	mov.l #80,r0
	shll r0
	mov.l r0,@(gbr,16)
	mov.l #0,r0
	mov.l r0,@(gbr,20)

	; can't do much during the divide wait, too bad.
	mov.l @(r4,4),r2  ; r2=v.y
	mov.l @(r4,0),r1  ; r1=v.x
	;neg r2,r2 ; neg here gives rounding differences

	; get results of hardware divide
	mov.l @(gbr,$1c),r0

	dmuls.l r1,r0
	sts mach,r3
	dmuls.l r2,r0
	mov.w r3,@r5
	sts mach,r0
	neg r0,r0
	rts
	mov.w r0,@(r5,2)

	.align 4
	littab

;rectTransform(Fixed32 wx,Fixed32 wy,Fixed32 wz,
; 	       int light,int h,int w,
;	       Fixed32 px,Fixed32 py,Fixed32 pz,
;	       Fixed32 hx,Fixed32 hy,Fixed32 hz,
;	       vCalc *output,
;	       unsigned short (*lightFunc)(int vLightIndex,
;				                  MthXyz *pos);

	xref level_vertexLight
	xref greyTable
rectTransform
	.align 4
	mov.l r14,@-r15
	mov.l r15,r14 ; r14 is gonna be the stack pointer
	mov.l r8,@-r15
	add.l #4,r14 ; r14 now points to "h" on the stack
	mov.l r9,@-r15
	mov.l r10,@-r15
	mov.l r11,@-r15
	mov.l r12,@-r15
	mov.l r13,@-r15
	sts.l pr,@-r15

	mov.l =level_vertexLight,r0
	mov.l @r0,r0
	add.l r0,r7
	; r4,r5,r6 = width vector
	; r7 = address of light array

	mov.l @(r14,8),r8
	mov.l @(r14,12),r9
	mov.l @(r14,16),r10
	; r8,r9,r10 = position
	mov.l @(r14,32),r12
	; r12=vcalc output pointer

	mov.l =$0ffffff00,r0
	ldc r0,gbr ; point gbr to hardware divider
	mov.l @(r14,36),r13
	; r13=light calc function or NULL if none needed
@heightLoop
	mov.l @(r14,4),r11
	; r11=width loop counter
@widthLoop
	; project point
	; ... compare z coordinate against F(33)
	mov.l #33,r1
	shll16 r1
	cmp/gt r1,r10
	movt r2
	bt/s @2
	mov.l r10,r0
	mov.l r1,r0
@2      ; ... r0 now holds the z coord or F(33), whichever is greater
	; ... start the hardware divide
	mov.l r0,@(gbr,0)
	mov.l r0,r3 ; save r0 for depth cue calculation below
	mov.l #80,r0
	shll r0
	mov.l r0,@(gbr,16)
	mov.l #0,r0
	mov.l r0,@(gbr,20)

	; compute light value with depth cueing
	mov.b @r7+,r1 ; get light value
	; ... do depth cueing calculations to modify r1
	; ... r3 = clamped z coord from above
	shlr16 r3
	shlr8 r3
	; stuff to support wavywalls
	;mov.l r1,r0
	;and #31,r0
	; end
	cmp/hi r1,r3
	bf/s @9
	sub r3,r1
	mov.l #0,r1
@9
        ; do light source computations if necessary
	tst r13,r13
	bf @callLit

	; otherwise do inline light
	mov.l =greyTable,r0
	shll r1
	mov.w @(r0,r1),r0  ; lookup in grey table
@retFromLit
	shll16 r2 ; test bit from way above
	shlr r2
	xor r2,r0 ; flip clip bit
	mov.w r0,@(r12,4) ; store vCalc.light

	add r6,r10 ; p->z+=width->z

	; get results of hardware divide
	mov.l @(gbr,$1c),r0

	dmuls.l r8,r0
	add r4,r8 ; p.x+=width.x
	sts mach,r3
	dmuls.l r9,r0
	mov.w r3,@r12 ; store vCalc.x
	add r5,r9 ; p.y+=width.y
	sts mach,r0
	neg r0,r0
	mov.w r0,@(r12,2) ; store vCalc.y

	dt r11
	bf/s @widthLoop
	add #6,r12

	; re-load position from memory
	mov.l @(r14,8),r8
	mov.l @(r14,12),r9
	mov.l @(r14,16),r10
	; add height vector
	mov.l @(r14,20),r0
	mov.l @(r14,24),r1
	add r0,r8
	mov.l @(r14,28),r2
	add r1,r9
	add r2,r10
	; and save it out
	mov.l r8,@(r14,8)
	mov.l r9,@(r14,12)
	mov.l r10,@(r14,16)

	mov.l @(r14,0),r0 ; h -> r0
	dt r0
	bf/s @heightLoop
	mov r0,@(r14,0)

	; donesville, pop the stack
	lds.l @r15+,pr
	mov.l @r15+,r13
	mov.l @r15+,r12
	mov.l @r15+,r11
	mov.l @r15+,r10
	mov.l @r15+,r9
	mov.l @r15+,r8
	rts
	mov.l @r15+,r14

	.align 4
	littab

	.align 4
@callLit ; perform function call to @r13
	;
	; ... save some regs
	mov.l r4,@-r15
	mov.l r5,@-r15
	mov.l r1,r4 ; r4=vlight value
	mov.l r6,@-r15
	mov.l r7,@-r15
	mov.l r2,@-r15
	; ... put pos on stack
	mov.l r10,@-r15
	mov.l r9,@-r15
	mov.l r8,@-r15
	jsr @r13 ; do call
	mov.l r15,r5 ; load pos address

	add #12,r15 ; pop pos off stack
	mov.l @r15+,r2
	mov.l @r15+,r7
	mov.l @r15+,r6
	mov.l @r15+,r5
	bra @retFromLit
	mov.l @r15+,r4



;normTransform(sVertexType *firstVertex,MthMatrix *viewMatrix,
;	       int nmVert,vCalc *output,
;	       unsigned short (*lightFunc)(int vLightIndex,
;				           MthXyz *pos);

	xref level_vertex
	xref greyTable
normTransform
	.align 4
	; r4=sVertexType *vertex
	; r5=MthMatrix *view
	; r6=nmVert left
	; r7=vCalc *output

	mov.l r10,@-r15
	mov.l r11,@-r15
	mov.l r12,@-r15
	mov.l r13,@-r15
	mov.l r14,@-r15
	mov.l @(r15,20),r13
	; r13=lightFunc
	sts.l pr,@-r15
	; reserve space for point
	mov #0,r0
	mov.l r0,@-r15
	mov.l r0,@-r15
	mov.l r0,@-r15
	mov r15,r14
	; r14 -> point on stack
	add #(8*4),r5 ; point r5 @ z coordinate
	mov.l =$0ffffff00,r0
	ldc r0,gbr ; point gbr to hardware divider
@vloop
	; copy vertex point from vertex array to point
	mov.l @r4+,r0
	mov.w r0,@(r14,4)
	swap.w r0,r0
	mov.w r0,@(r14,0)
	mov.w @r4+,r0
	clrmac
	mov.w r0,@(r14,8)
	; find the transformed z coordinate
	mac.l @r5+,@r14+
	mac.l @r5+,@r14+
	mac.l @r5+,@r14+
	sts.l mach,r1
	sts.l macl,r0
	xtrct r1,r0
	mov.l @r5+,r1
	add r1,r0
	; z coordinate is now in r0
	mov.l #33,r1
	shll16 r1
	cmp/gt r1,r0
	movt r2
	bt/s @2
	mov r0,r10 ; copy of z coordinate in r10
	mov.l r1,r0
@2      ; ... r0 now holds the z coord or F(33), whichever is greater
	; ... start the hardware divide
	mov.l r0,@(gbr,0)
	mov.l #80,r0
	shll r0
	mov.l r0,@(gbr,16)
	mov.l #0,r0
	mov.l r0,@(gbr,20)

	; transform other coordinates
	clrmac
	sub #(12*4),r5 ; put r5 back to front of matrix
	sub #(3*4),r14 ; put r14 back to front of points
	mac.l @r5+,@r14+
	mac.l @r5+,@r14+
	mac.l @r5+,@r14+
	sts.l mach,r1
	sts.l macl,r11
	xtrct r1,r11
	mov.l @r5+,r1
	add r1,r11
	; r11 now holds transformed x coordinate
	clrmac
	sub #(3*4),r14 ; put r14 back to front of points
	mac.l @r5+,@r14+
	mac.l @r5+,@r14+
	mac.l @r5+,@r14+
	sub #(3*4),r14 ; put r14 back to front of points
	sts.l mach,r1
	sts.l macl,r12
	xtrct r1,r12
	mov.l @r5+,r1
	add r1,r12
	; r12 now holds transformed y coordinate


	; compute light value with depth cueing
	mov.b @r4+,r1 ; get light value
	; ... do depth cueing calculations to modify r1
	; ... r10 = clamped z coord from above
	mov.l r10,r3
	shlr16 r3
	shlr8 r3
;	shlr2 r3
;	shlr2 r3
;	shlr r3
	cmp/hi r1,r3
	bf/s @9
	sub r3,r1
	mov.l #0,r1
@9
        ; do light source computations if necessary
	tst r13,r13
	bf @callLit

	; otherwise do inline light
	mov.l =greyTable,r0
	shll r1
	mov.w @(r0,r1),r0  ; lookup in grey table
@retFromLit
	shll16 r2 ; test bit from way above
	shlr r2
	xor r2,r0 ; flip clip bit
;	mov.l =$7fff,r0 ; temp
	mov.w r0,@(r7,4) ; store vCalc.light
	add #1,r4 ; skip r4 past padding

	; time to fetch the divide result
	mov.l @(gbr,$1c),r0

	dmuls.l r11,r0
	sts mach,r1
	dmuls.l r12,r0
	mov.w r1,@r7 ; store vCalc.x
	sts mach,r0
	neg r0,r0
	mov.w r0,@(r7,2) ; store vCalc.y

	dt r6
	bf/s @vloop
	add #6,r7 ; point r7 to next vCalc entry

	add #(3*4),r15 ; pop point space off stack
	lds.l @r15+,pr
	mov.l @r15+,r14
	mov.l @r15+,r13
	mov.l @r15+,r12
	mov.l @r15+,r11
	rts
	mov.l @r15+,r10



@callLit ; perform function call to @r13
	;
	; ... save some regs
	mov.l r4,@-r15
	mov.l r1,r4 ; vlight value
	mov.l r5,@-r15
	mov.l r6,@-r15
	mov.l r7,@-r15
	mov.l r2,@-r15
	; ... put pos on stack
	mov.l r10,@-r15
	mov.l r12,@-r15
	mov.l r11,@-r15
	jsr @r13 ; do call
	mov.l r15,r5 ; load pos address

	add #12,r15 ; pop pos off stack
	mov.l @r15+,r2
	mov.l @r15+,r7
	mov.l @r15+,r6
	mov.l @r15+,r5
	bra @retFromLit
	mov.l @r15+,r4
