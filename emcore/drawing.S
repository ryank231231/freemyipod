@
@
@    Copyright 2010 TheSeven
@
@
@    This file is part of emCORE.
@
@    emCORE is free software: you can redistribute it and/or
@    modify it under the terms of the GNU General Public License as
@    published by the Free Software Foundation, either version 2 of the
@    License, or (at your option) any later version.
@
@    emCORE is distributed in the hope that it will be useful,
@    but WITHOUT ANY WARRANTY; without even the implied warranty of
@    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
@    See the GNU General Public License for more details.
@
@    You should have received a copy of the GNU General Public License
@    along with emCORE.  If not, see <http://www.gnu.org/licenses/>.
@
@


#define ASM_FILE
#include "global.h"


.section .icode.get_font_width, "ax", %progbits
.align 2
.global get_font_width
.type get_font_width, %function
get_font_width:
	mov	r0, #6
	mov	pc, lr
.size get_font_width, .-get_font_width


.section .icode.get_font_height, "ax", %progbits
.align 2
.global get_font_height
.type get_font_height, %function
get_font_height:
	mov	r0, #8
	mov	pc, lr
.size get_font_height, .-get_font_height


.section .icode.rendertext, "ax", %progbits
.align 2
.global rendertext
.type rendertext, %function
rendertext:
	stmfd	sp!, {r1-r8,lr}
	mov	r4, r0
	ldmfd	sp, {r5-r7}
	ldr	r0, [sp,#0x24]
	ldr	r1, [sp,#0x28]
	ldr	r8, [sp,#0x2c]
	stmia	sp, {r0,r1}
rendertext_loop:
	ldrb	r12, [r8], #1
	mov	r0, r4
	cmp	r12, #0
	mov	r1, r5
	beq	rendertext_done
	mov	r2, r6
	str	r12, [sp,#0x8]
	mov	r3, r7
	bl	renderchar
	add	r5, r5, #6
	b	rendertext_loop
rendertext_done:
	ldmfd	sp!, {r1-r8,pc}
.size rendertext, .-rendertext


.section .icode.renderchar, "ax", %progbits
.align 2
.global renderchar
.type renderchar, %function
.global renderchar_native
.type renderchar_native, %function
renderchar:
	stmfd	sp!, {r4-r9,lr}
	mla	r1, r2, r3, r1
	ldr	r2, [sp,#0x24]
	add	r1, r1, r1,lsl#1
	sub	r2, r2, #0x20
	add	r0, r0, r1
	adr	r7, renderchar_font
	cmp	r2, #0x5f
	add	r2, r2, r2,lsl#2
	mov	r1, #6
	addcc	r7, r7, r2
	ldr	r4, [sp,#0x1c]
	add	r3, r3, r3,lsl#1
	ldr	r5, [sp,#0x20]
renderchar_x:
	cmp	r1, #1
	mov	r2, #8
	ldrneb	lr, [r7], #1
renderchar_y:
	mov	r12, r5,lsr#24
	and	r9, r5, #0xff
	ldrb	r8, [r0]
	mul	r9, r12, r9
	rsb	r12, r12, #0xff
	tst	lr, #1
	mla	r8, r12, r8, r9
	movne	r12, r4,lsr#24
	andne	r9, r4, #0xff
	mov	r8, r8,lsr#8
	mulne	r9, r12, r9
	rsbne	r12, r12, #0xff
	mov	lr, lr,lsr#1
	mlane	r8, r12, r8, r9
	mov	r9, r5,lsr#8
	movne	r8, r8,lsr#8
	mov	r12, r5,lsr#24
	strb	r8, [r0]
	and	r9, r9, #0xff
	ldrb	r8, [r0,#1]
	mul	r9, r12, r9
	rsb	r12, r12, #0xff
	mla	r8, r12, r8, r9
	movne	r9, r4,lsr#8
	movne	r12, r4,lsr#24
	andne	r9, r9, #0xff
	mov	r8, r8,lsr#8
	mulne	r9, r12, r9
	rsbne	r12, r12, #0xff
	mlane	r8, r12, r8, r9
	mov	r12, r5,lsr#24
	mov	r9, r5,lsr#16
	movne	r8, r8,lsr#8
	and	r9, r9, #0xff
	strb	r8, [r0,#1]
	mul	r9, r12, r9
	ldrb	r8, [r0,#2]
	rsb	r12, r12, #0xff
	movne	r9, r4,lsr#16
	mla	r8, r12, r8, r9
	andne	r9, r9, #0xff
	movne	r12, r4,lsr#24
	mov	r8, r8,lsr#8
	mulne	r9, r12, r9
	rsbne	r12, r12, #0xff
	mlane	r8, r12, r8, r9
	movne	r8, r8,lsr#8
	subs	r2, r2, #1
	strb	r8, [r0,#2]
	add	r0, r0, r3
	bne	renderchar_y
	sub	r0, r0, r3,lsl#3
	subs	r1, r1, #1
	add	r0, r0, #3
	bne	renderchar_x
	ldmfd	sp!, {r4-r9,pc}

renderchar_native:
#if (LCD_BYTESPERPIXEL == 2)
	stmfd	sp!, {r4-r7,lr}
	ldr	r7, [sp,#0x14]
	cmn	r2, #1
	beq	renderchar_native_nobg
	mov	r6, r0
	mov	r4, #8
	mov	lr, r2,lsr#16
	sub	lr, lr, #0xff00
	subs	lr, lr, #0xff
	bne	renderchar_native_opaquerow
	mov	r12, r2,lsl#16
	mov	r12, r12,lsr#16
renderchar_native_blendrow:
	mov	r5, #6
renderchar_native_blendcol:
	ldrh	lr, [r6]
	movs	lr, lr,lsr#1
#ifdef LCD_BIGENDIAN
	orrcs	lr, lr, #0x10000
	bic	lr, lr, #0x1000
	bic	lr, lr, #0x84
#else
	bic	lr, lr, #0x410
#endif
	add	lr, lr, r12
	strh	lr, [r6], #2
	subs	r5, r5, #1
	bne	renderchar_native_blendcol
	add	r6, r6, r7,lsl#1
	sub	r6, r6, #12
	subs	r4, r4, #1
	bne	renderchar_native_blendrow
	b	renderchar_native_nobg
renderchar_native_opaquerow:
	mov	r5, #6
renderchar_native_opaquecol:
	strh	r2, [r6], #2
	subs	r5, r5, #1
	bne	renderchar_native_opaquecol
	add	r6, r6, r7,lsl#1
	sub	r6, r6, #12
	subs	r4, r4, #1
	bne	renderchar_native_opaquerow
renderchar_native_nobg:
	adr	r5, renderchar_font
	sub	r3, r3, #0x20
	cmp	r3, #0x5f
	addcc	r5, r3,lsl#2
	addcc	r5, r3
	mov	r3, #5
renderchar_native_col:
	mov	r6, r0
	ldrb	r4, [r5], #1
renderchar_native_row:
	tst	r4, #1
	strneh	r1, [r6]
	add	r6, r6, r7,lsl#1
	movs	r4, r4,lsr#1
	bne	renderchar_native_row
	add	r0, r0, #2
	subs	r3, r3, #1
	bne	renderchar_native_col
	add	r0, r0, #2
	ldmfd	sp!, {r4-r7,pc}
#else
#if (LCD_BYTESPERPIXEL == 4)

	stmfd	sp!, {r4-r7,lr}
	ldr	r7, [sp,#0x14]
	cmn	r2, #1
	beq	renderchar_native_nobg
	mov	r6, r0
	mov	r4, #8
renderchar_native_opaquerow:
	mov	r5, #6
renderchar_native_opaquecol:
	str	r2, [r6], #4
	subs	r5, r5, #1
	bne	renderchar_native_opaquecol
	add	r6, r6, r7,lsl#1
	sub	r6, r6, #12
	subs	r4, r4, #1
	bne	renderchar_native_opaquerow
renderchar_native_nobg:
	adr	r5, renderchar_font
	sub	r3, r3, #0x20
	cmp	r3, #0x5f
	addcc	r5, r3,lsl#2
	addcc	r5, r3
	mov	r3, #5
renderchar_native_col:
	mov	r6, r0
	ldrb	r4, [r5], #1
renderchar_native_row:
	tst	r4, #1
	strne	r1, [r6]
	add	r6, r6, r7,lsl#2
	movs	r4, r4,lsr#1
	bne	renderchar_native_row
	add	r0, r0, #4
	subs	r3, r3, #1
	bne	renderchar_native_col
	add	r0, r0, #4
	ldmfd	sp!, {r4-r7,pc}
#else
#error Unknown number of bytes per pixel!
#endif
#endif

renderchar_font:
	.byte 0, 0, 0, 0, 0
	.byte 0, 0, 95, 0, 0
	.byte 0, 7, 0, 7, 0
	.byte 20, 127, 20, 127, 20
	.byte 36, 42, 127, 42, 18
	.byte 35, 19, 8, 100, 98
	.byte 54, 73, 85, 34, 80
	.byte 5, 3, 0, 0, 0
	.byte 28, 34, 65, 0, 0
	.byte 0, 0, 65, 34, 28
	.byte 20, 8, 62, 8, 20
	.byte 8, 8, 62, 8, 8
	.byte 0, -96, 96, 0, 0
	.byte 8, 8, 8, 8, 8
	.byte 0, 96, 96, 0, 0
	.byte 32, 16, 8, 4, 2
	.byte 62, 81, 73, 69, 62
	.byte 0, 66, 127, 64, 0
	.byte 66, 97, 81, 73, 70
	.byte 33, 65, 69, 75, 49
	.byte 24, 20, 18, 127, 16
	.byte 39, 69, 69, 69, 57
	.byte 60, 74, 73, 73, 48
	.byte 1, 113, 9, 5, 3
	.byte 54, 73, 73, 73, 54
	.byte 6, 73, 73, 41, 30
	.byte 0, 54, 54, 0, 0
	.byte 0, 86, 54, 0, 0
	.byte 8, 20, 34, 65, 0
	.byte 20, 20, 20, 20, 20
	.byte 0, 65, 34, 20, 8
	.byte 2, 1, 81, 9, 6
	.byte 50, 73, 121, 65, 62
	.byte 124, 18, 17, 18, 124
	.byte 127, 73, 73, 73, 62
	.byte 62, 65, 65, 65, 34
	.byte 127, 65, 65, 34, 28
	.byte 127, 73, 73, 73, 65
	.byte 127, 9, 9, 9, 1
	.byte 62, 65, 73, 73, 58
	.byte 127, 8, 8, 8, 127
	.byte 0, 65, 127, 65, 0
	.byte 32, 64, 65, 63, 1
	.byte 127, 8, 20, 34, 65
	.byte 127, 64, 64, 64, 64
	.byte 127, 2, 12, 2, 127
	.byte 127, 4, 8, 16, 127
	.byte 62, 65, 65, 65, 62
	.byte 127, 9, 9, 9, 6
	.byte 62, 65, 81, 33, 94
	.byte 127, 9, 25, 41, 70
	.byte 38, 73, 73, 73, 50
	.byte 1, 1, 127, 1, 1
	.byte 63, 64, 64, 64, 63
	.byte 31, 32, 64, 32, 31
	.byte 127, 32, 24, 32, 127
	.byte 99, 20, 8, 20, 99
	.byte 3, 4, 120, 4, 3
	.byte 97, 81, 73, 69, 67
	.byte 0, 127, 65, 65, 0
	.byte 2, 4, 8, 16, 32
	.byte 0, 65, 65, 127, 0
	.byte 4, 2, 1, 2, 4
	.byte 64, 64, 64, 64, 64
	.byte 1, 2, 4, 0, 0
	.byte 32, 84, 84, 84, 120
	.byte 127, 68, 68, 68, 56
	.byte 56, 68, 68, 68, 40
	.byte 56, 68, 68, 68, 127
	.byte 56, 84, 84, 84, 24
	.byte 8, 126, 9, 1, 2
	.byte 8, 84, 84, 84, 60
	.byte 127, 4, 4, 4, 120
	.byte 0, 68, 125, 64, 0
	.byte 32, 64, 64, 61, 0
	.byte 127, 16, 40, 68, 0
	.byte 0, 65, 127, 64, 0
	.byte 124, 4, 24, 4, 120
	.byte 124, 8, 4, 4, 120
	.byte 56, 68, 68, 68, 56
	.byte 124, 20, 20, 20, 24
	.byte 8, 20, 20, 20, 124
	.byte 124, 8, 4, 4, 8
	.byte 72, 84, 84, 84, 32
	.byte 4, 63, 68, 64, 32
	.byte 60, 64, 64, 32, 124
	.byte 28, 32, 64, 32, 28
	.byte 60, 64, 56, 64, 60
	.byte 68, 40, 16, 40, 68
	.byte 12, 80, 80, 80, 60
	.byte 68, 100, 84, 76, 68
	.byte 0, 8, 54, 65, 0
	.byte 0, 0, 119, 0, 0
	.byte 0, 65, 54, 8, 0
	.byte 2, 1, 2, 4, 2
.align 2
.size renderchar_native, .-renderchar_native
