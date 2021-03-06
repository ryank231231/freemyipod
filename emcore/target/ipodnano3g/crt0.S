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


.section .intvect,"ax",%progbits
	ldr pc, =reset_handler
	ldr pc, =undef_instr_handler
	ldr pc, =syscall_handler
	ldr pc, =prefetch_abort_handler
	ldr pc, =data_abort_handler
	ldr pc, =reserved_handler
	ldr pc, =irq_handler
	ldr pc, =fiq_handler
.ltorg


.section .inithead,"ax",%progbits
.global __start
__start:
	b	_start

.section .initcode,"ax",%progbits
.global _start
_start:
	mrc	p15, 0, r0,c1,c0
	bic	r0, r0, #0x200
	orr	r0, r0, #0x100
	mcr	p15, 0, r0,c1,c0
	mov	r0, #0x7fffffff
	mcr	p15, 0, r0,c3,c0
	mov	r0, #0x22000000
	orr	r1, r0, #0x00000100
	orr	r0, r0, #0x0003c000
	orr	r1, r1, #0x000000fe
	add	r2, r0, #0x200
	mov	r3, #0
	str	r1, [r0], #4
.mmuloop1:
	str	r3, [r0], #4
	cmp	r0, r2
	bne	.mmuloop1
	add	r0, r0, #0x100
	add	r2, r0, #0x500
.mmuloop2:
	str	r3, [r0], #4
	cmp	r0, r2
	bne	.mmuloop2
	add	r0, r0, #0x4
	add	r2, r0, #0x7c
.mmuloop3:
	str	r3, [r0], #4
	cmp	r0, r2
	bne	.mmuloop3
	add	r0, r0, #0x4
	add	r2, r0, #0x500
	add	r2, r2, #0x7c
.mmuloop4:
	str	r3, [r0], #4
	cmp	r0, r2
	bne	.mmuloop4
	add	r0, r0, #0x200
	add	r2, r0, #0x3000
.mmuloop5:
	str	r3, [r0], #4
	cmp	r0, r2
	bne	.mmuloop5
	mrc	p15, 0, r0,c1,c0
	orr	r0, r0, #5
	mcr	p15, 0, r0,c1,c0
	ldr	r0, =_sramsource
	ldr	r1, =_sramstart
	ldr	r2, =_sramend
.copysram:
	cmp	r2, r1
	ldrhi	r3, [r0], #4
	strhi	r3, [r1], #4
	bhi	.copysram
	ldr	r0, =_sdramsource
	ldr	r1, =_sdramstart
	ldr	r2, =_sdramend
.copysdram:
	cmp	r2, r1
	ldrhi	r3, [r0], #4
	strhi	r3, [r1], #4
	bhi	.copysdram
	ldr	r0, =_ibssstart
	ldr	r1, =_ibssend
	mov	r2, #0
.clearibss:
	cmp	r1, r0
	strhi	r2, [r0], #4
	bhi	.clearibss
	ldr	r0, =_bssstart
	ldr	r1, =_bssend
.clearbss:
	cmp	r1, r0
	strhi	r2, [r0], #4
	bhi	.clearbss
	ldr	r1, =0x38200000
	ldr	r0, [r1]
	orr	r0, r0, #1
	bic	r0, r0, #0x10000
	str	r0, [r1]
.cleancache:
	mrc	p15, 0, r15,c7,c10,3
	bne	.cleancache
	mov	r0, #0
	mcr	p15, 0, r0,c7,c10,4
	mcr	p15, 0, r0,c7,c5,0
	ldr	r1, =0x38200000
	ldr	r0, [r1]
	orr	r0, r0, #1
	bic	r0, r0, #0x10000
	str	r0, [r1]
	mov	r0, #0
	mcr	p15, 0, r0,c7,c5,0
	add	r1, r1, #0x00c00000
	add	r2, r1, #0x00001000
	add	r3, r1, #0x00002000
	sub	r4, r0, #1
	str	r4, [r1,#0x14]
	str	r4, [r2,#0x14]
	str	r4, [r1,#0xf00]
	str	r4, [r2,#0xf00]
	str	r4, [r3,#0x08]
	str	r4, [r3,#0x0c]
	str	r0, [r1,#0x14]
	str	r0, [r2,#0x14]
	msr	cpsr_c, #0xd2
	ldr	sp, =_irqstackend
	msr	cpsr_c, #0xd7
	ldr	sp, =_abortstackend
	msr	cpsr_c, #0xdb
	ldr	sp, =_abortstackend
	msr	cpsr_c, #0x1f
	ldr	sp, =_abortstackend
	bl	init
	bl	yield
	mov	r0, #0
	ldr	pc, =idleloop
.ltorg


.section .icode, "ax", %progbits
.align 2
idleloop:
	mcr	p15, 0, r0,c7,c0,4
	b	idleloop

.global reset
.global hang
.type reset, %function
.type hang, %function
reset:
	msr	cpsr_c, #0xd3
	mov	r0, #0x100000
	mov	r1, #0x3c800000
	str	r0, [r1]
hang:
	msr	cpsr_c, #0xd3
	mov	r0, #0
	mcr	p15, 0, r0,c7,c0,4
	b	hang
.size reset, .-reset
.size hang, .-hang

.type reset_handler, %function
reset_handler:
	stmfd	sp, {r10-r12}
	mov	r10, sp
	mov	r11, lr
	mrs	r12, cpsr
	msr	cpsr_c, #0xd7
	sub	sp, sp, #0x44
	stmia	sp!, {r0-r9}
	sub	r0, r10, #0xc
	ldmia	r0, {r0-r2}
	mov	r3, r10
	mov	r4, r11
	mov	r5, r11
	mov	r6, r12
	stmia	sp!, {r0-r6}
	sub	sp, sp, #0x44
	mov	r0, #0
	adr	r1, reset_text
	mov	r2, r11
	b	panicf
.size reset_handler, .-reset_handler

.global undef_instr_handler
.type undef_instr_handler, %function
undef_instr_handler:
	sub	sp, sp, #0x44
	stmia	sp!, {r0-r12}
	sub	r2, lr, #4
	mrs	r3, spsr
	mrs	r4, cpsr
	orr	r0, r3, #0xc0
	msr	cpsr_c, r0
	mov	r0, sp
	mov	r1, lr
	msr	cpsr_c, r4
	stmia	sp!, {r0-r3}
	sub	sp, sp, #0x44
	mov	r0, #0
	adr	r1, undef_instr_text
	ldr	r3, [r2]
	b	panicf
.size undef_instr_handler, .-undef_instr_handler

.type prefetch_abort_handler, %function
prefetch_abort_handler:
	sub	sp, sp, #0x44
	stmia	sp!, {r0-r12}
	sub	r2, lr, #4
	mrs	r3, spsr
	mrs	r4, cpsr
	orr	r0, r3, #0xc0
	msr	cpsr_c, r0
	mov	r0, sp
	mov	r1, lr
	msr	cpsr_c, r4
	stmia	sp!, {r0-r3}
	sub	sp, sp, #0x44
	mov	r0, #0
	adr	r1, prefetch_abort_text
	mrc	p15, 0, r3,c5,c0
	mov	r4, r3,lsr#4
	and	r4, r4, #0xf
	and	r5, r3, #0xf
	stmfd	sp!, {r4-r5}
	b	panicf
.size prefetch_abort_handler, .-prefetch_abort_handler

.type data_abort_handler, %function
data_abort_handler:
	sub	sp, sp, #0x44
	stmia	sp!, {r0-r12}
	sub	r2, lr, #8
	mrs	r3, spsr
	mrs	r4, cpsr
	orr	r0, r3, #0xc0
	msr	cpsr_c, r0
	mov	r0, sp
	mov	r1, lr
	msr	cpsr_c, r4
	stmia	sp!, {r0-r3}
	sub	sp, sp, #0x44
	mov	r0, #0
	adr	r1, data_abort_text
	mrc	p15, 0, r3,c5,c0
	mov	r4, r3,lsr#4
	and	r4, r4, #0xf
	and	r5, r3, #0xf
	mrc	p15, 0, r6,c6,c0
	stmfd	sp!, {r4-r6}
	b	panicf
.size data_abort_handler, .-data_abort_handler

.type reserved_handler, %function
reserved_handler:
	stmfd	sp, {r10-r12}
	mov	r10, sp
	mov	r11, lr
	mrs	r12, cpsr
	msr	cpsr_c, #0xd7
	sub	sp, sp, #0x44
	stmia	sp!, {r0-r9}
	sub	r0, r10, #0xc
	ldmia	r0, {r0-r2}
	mov	r3, r10
	mov	r4, r11
	mov	r5, r11
	mov	r6, r12
	stmia	sp!, {r0-r6}
	sub	sp, sp, #0x44
	mov	r0, #0
	adr	r1, reserved_text
	mov	r2, r11
	b	panicf
.size reserved_handler, .-reserved_handler

.type fiq_handler, %function
fiq_handler:
	mov	r0, #2
	adr	r1, fiq_text
	b	panic
.size fiq_handler, .-fiq_handler

prefetch_abort_text:
	.ascii	"Prefetch abort at %08X!\nFSR: %08X (domain %d, fault %d)\0"

reset_text:
	.ascii	"Hit reset vector!\n(Last known PC: %08X)\0"

undef_instr_text:
	.ascii	"Undefined instruction at %08X!\n(Opcode: %08X)\0"

data_abort_text:
	.ascii	"Data abort at %08X!\nFSR: %08X (domain %d, fault %d)\nAddress: %08X\0"

fiq_text:
	.ascii	"Unhandled FIQ!\0"

reserved_text:
	.ascii	"Hit reserved exception handler!\n(Last known PC: %08X)\0"

syscall_text:
	.ascii	"Unhandled syscall!\0"


.section .icode.usec_timer, "ax", %progbits
.align 2
.global read_native_timer
.type read_native_timer, %function
read_native_timer:
	ldr	r0, val_3c700000
	ldr	r1, [r0,#0xb4]
	mov	r0, #0
	bx	lr
.size read_native_timer, .-read_native_timer

.global read_usec_timer
.type read_usec_timer, %function
read_usec_timer:
	ldr	r0, val_3c700000
	ldr	r0, [r0,#0xb4]
	bx	lr
.size read_usec_timer, .-read_usec_timer

val_3c700000:
	.word	0x3c700000
