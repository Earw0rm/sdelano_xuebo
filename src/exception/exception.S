
#include "exception.h"
#include "arm/arm_util.h"
#include "memlayout.h"

.section ".text"
// ***************************************
// Exceptiom vector table, Page 5357 of AArch64-Reference-Manual (A-profile architecture reference manual).    
// align 7 sinse each level of exceptions in 0x80 border
// esr_el1 => exception syndrome register. 
// elr_el1 => exception link register
// ***************************************
.macro ventry label
.align 7
    b \label
.endm



.macro save_uctx
	sub sp, sp, #16
	str x0, [sp]
	str x1, [sp, #-8]

	ldr x0, #LAYOUT_MY_CPU_BASE
	ldr x0, [x0]

	mrs x1, mpidr_el1
	and x1, x1, #0xff
	lsl x1, x1, #12

	add x0, x1 

	stp	x2, x3,   [x0, #16 * 1]
	stp	x4, x5,   [x0, #16 * 2]
	stp	x6, x7,   [x0, #16 * 3]
	stp	x8, x9,   [x0, #16 * 4]
	stp	x10, x11, [x0, #16 * 5]
	stp	x12, x13, [x0, #16 * 6]
	stp	x14, x15, [x0, #16 * 7]
	stp	x16, x17, [x0, #16 * 8]
	stp	x18, x19, [x0, #16 * 9]
	stp	x20, x21, [x0, #16 * 10]
	stp	x22, x23, [x0, #16 * 11]
	stp	x24, x25, [x0, #16 * 12]
	stp	x26, x27, [x0, #16 * 13]
	stp	x28, x29, [x0, #16 * 14]
	
	mov x2, x0

	ldr x0, [sp]
	ldr x1, [sp, #-8]
	add sp, sp, #16

	stp x0, x1, [x2, #16 * 0]
	
	mrs x0, sp_el0
	stp x30, x0, [x2, #16 * 15]

	mrs x0, elr_el1
	mrs x1, spsr_el1 
	stp x0, x1, [x2, #16 * 16]

	mrs x0, esr_el1
	mrs x1, far_el1 
	stp x0, x1, [x2, #16 * 17]
.endm

.macro restore_uctx
	ldr x0, #LAYOUT_MY_CPU_BASE
	ldr x0, [x0]

	mrs x1, mpidr_el1
	and x1, x1, #0xff
	lsl x1, x1, #12

	add x0, x1 

	ldp	x2, x3,   [x0, #16 * 1]
	ldp	x4, x5,   [x0, #16 * 2]
	ldp	x6, x7,   [x0, #16 * 3]
	ldp	x8, x9,   [x0, #16 * 4]
	ldp	x10, x11, [x0, #16 * 5]
	ldp	x12, x13, [x0, #16 * 6]
	ldp	x14, x15, [x0, #16 * 7]
	ldp	x16, x17, [x0, #16 * 8]
	ldp	x18, x19, [x0, #16 * 9]
	ldp	x20, x21, [x0, #16 * 10]
	ldp	x22, x23, [x0, #16 * 11]
	ldp	x24, x25, [x0, #16 * 12]
	ldp	x26, x27, [x0, #16 * 13]
	ldp	x28, x29, [x0, #16 * 14]

	ldr x1, [x0, #248] // 16 * 15 + 8
	msr sp_el0, x1 

	ldr x1, [x0, #256] // 16 * 15 + 16
	msr elr_el1, x1

	ldr x1, [x0, #264] // 16 * 15 + 24
	msr spsr_el1, x1

	ldr x1, [x0, #272] // 16 * 15 + 32
	msr esr_el1, x1

	ldr x1, [x0, #280] // 16 * 15 + 40
	msr far_el1, x1

	ldr x1, [x0, #8]
	ldr x0, [x0, #0]
.endm


// таблица векторов обязана быть выровненной по 2^11
.align 11 
.globl vectors
vectors:
	ventry	invalid_ex			// Synchronous EL1t
	ventry	invalid_ex			// IRQ EL1t
	ventry	invalid_ex			// FIQ EL1t
	ventry	invalid_ex			// Error EL1t

	ventry	invalid_ex			// Synchronous EL1h
	ventry	el1_irq					    // IRQ EL1h
	ventry	invalid_ex			// FIQ EL1h
	ventry	invalid_ex			// Error EL1h

	ventry	el0_sync	         // Synchronous 64-bit EL0
	ventry	invalid_ex	 // IRQ 64-bit EL0
	ventry	invalid_ex   // FIQ 64-bit EL0
	ventry	invalid_ex // Error 64-bit EL0

	ventry	invalid_ex // Synchronous 32-bit EL0
	ventry	invalid_ex  // IRQ 32-bit EL0
	ventry	invalid_ex  // FIQ 32-bit EL0
	ventry	invalid_ex  // Error 32-bit EL0


invalid_ex:
	mrs x15, far_el1
w:
	b w 

el0_sync:
	save_uctx
	bl	handle_el0_sync
	restore_uctx
	eret


// for sys.S
.globl call_table_and_ret
call_table_and_ret:
    blr x8
    mov x0, x25        // just tmp save
    msr daifset, #2
    str x25, [sp, #16] //     str   x0, [sp, #S_X0]             // returned x0
    restore_uctx

    //TODO error here 
    ret

//////////////////////////////////////////////////////////////////////////////////////////////


.globl urestore_and_ret
urestore_and_ret:
	restore_uctx
	eret
	
el1_irq:
	save_uctx
	bl	handle_irq
	restore_uctx
	eret 

///////////////////////////////////////////////////////////////////////
.align 11 
.globl el3_vec
el3_vec:
	ventry	el3_panic			// Synchronous EL1t
	ventry	el3_panic			// IRQ EL1t
	ventry	el3_panic			// FIQ EL1t
	ventry	el3_panic			// Error EL1t

	ventry	el3_panic			// Synchronous EL1h
	ventry	el3_panic			// IRQ EL1h
	ventry	el3_panic			// FIQ EL1h
	ventry	el3_panic			// Error EL1h

	ventry	el3_panic	         // Synchronous 64-bit EL0
	ventry	el3_panic	 		// IRQ 64-bit EL0
	ventry	el3_panic  			// FIQ 64-bit EL0
	ventry	el3_panic 			// Error 64-bit EL0

	ventry	el3_panic 			// Synchronous 32-bit EL0
	ventry	el3_panic  			// IRQ 32-bit EL0
	ventry	el3_panic  			// FIQ 32-bit EL0
	ventry	el3_panic  			// Error 32-bit EL0


el3_panic:
	b el3_panic
