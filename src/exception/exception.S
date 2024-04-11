
#include "exception.h"
#include "arm/arm_util.h"

.extern LAYOUT_MY_CPU

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

//in this context, sp it is KERNEL stack. Mean that if kernel in h regime, then sp is sp_el1.
.macro kernel_entry
	sub	sp, sp,   #256
	stp	x0, x1,   [sp, #16 * 0]
	stp	x2, x3,   [sp, #16 * 1]
	stp	x4, x5,   [sp, #16 * 2]
	stp	x6, x7,   [sp, #16 * 3]
	stp	x8, x9,   [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]
	
	mrs x21, sp_el0
	mrs x22, elr_el1 
	mrs x23, spsr_el1

	stp x30, x21, [sp, #16 * 15]
	stp x22, x23, [sp, #16 * 16]	
.endm

.macro kernel_exit  
	ldp x30, x21, [sp, #16 * 15]
	ldp x22, x23, [sp, #16 * 16]

	msr sp_el0, x21
	msr elr_el1,  x22 
	msr spsr_el1, x23

	ldp	x0, x1,   [sp, #16 * 0]
	ldp	x2, x3,   [sp, #16 * 1]
	ldp	x4, x5,   [sp, #16 * 2]
	ldp	x6, x7,   [sp, #16 * 3]
	ldp	x8, x9,   [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	ldr	x30,      [sp, #16 * 15] 
	add	sp, sp, #256
	eret
.endm



.macro handle_invalid_entry type 
    kernel_entry 
    mov x0, #\type
    mrs x1, esr_el1
    mrs x2, elr_el1
    bl show_invalid_entry_message // panic
    b err_hang
.endm




// таблица векторов обязана быть выровненной по 2^11
.align 11 
.globl vectors
vectors:
	ventry	sync_invalid_el1t			// Synchronous EL1t
	ventry	irq_invalid_el1t			// IRQ EL1t
	ventry	fiq_invalid_el1t			// FIQ EL1t
	ventry	error_invalid_el1t			// Error EL1t

	ventry	sync_invalid_el1h			// Synchronous EL1h
	ventry	el1_irq					    // IRQ EL1h
	ventry	fiq_invalid_el1h			// FIQ EL1h
	ventry	error_invalid_el1h			// Error EL1h

	ventry	el0_sync	         // Synchronous 64-bit EL0
	ventry	irq_invalid_el0_64	 // IRQ 64-bit EL0
	ventry	fiq_invalid_el0_64   // FIQ 64-bit EL0
	ventry	error_invalid_el0_64 // Error 64-bit EL0

	ventry	sync_invalid_el0_32 // Synchronous 32-bit EL0
	ventry	irq_invalid_el0_32  // IRQ 32-bit EL0
	ventry	fiq_invalid_el0_32  // FIQ 32-bit EL0
	ventry	error_invalid_el0_32  // Error 32-bit EL0


sync_invalid_el1t:
	handle_invalid_entry SYNC_INVALID_EL1t

irq_invalid_el1t:
	handle_invalid_entry IRQ_INVALID_EL1t

fiq_invalid_el1t:
	handle_invalid_entry FIQ_INVALID_EL1t

error_invalid_el1t:
	handle_invalid_entry ERROR_INVALID_EL1t

sync_invalid_el1h:	 
 	mrs x15, far_el1
w:
	b w // for debug purpose



fiq_invalid_el1h:
	handle_invalid_entry FIQ_INVALID_EL1h

error_invalid_el1h:
	handle_invalid_entry ERROR_INVALID_EL1h

sync_invalid_el0_64:
	handle_invalid_entry SYNC_INVALID_EL0_64

irq_invalid_el0_64:
	handle_invalid_entry IRQ_INVALID_EL0_64

fiq_invalid_el0_64:
	handle_invalid_entry FIQ_INVALID_EL0_64

error_invalid_el0_64:
	handle_invalid_entry ERROR_INVALID_EL0_64

sync_invalid_el0_32:
	handle_invalid_entry SYNC_INVALID_EL0_32

irq_invalid_el0_32:
	handle_invalid_entry IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
	handle_invalid_entry FIQ_INVALID_EL0_32

error_invalid_el0_32:
	handle_invalid_entry ERROR_INVALID_EL0_32

// boolsheet, peredelat'
el1_irq:
	kernel_entry
	bl	handle_irq
	kernel_exit

el0_sync:
	kernel_entry
	bl	handle_el0_sync
	kernel_exit

.globl err_hang
err_hang: b err_hang

//vbar_el1 => vector base address register 
//daif => part of the pstate
.globl irq_vector_init
irq_vector_init:
    adr x0, vectors
    msr vbar_el1, x0 
    ret 


// for sys.S
.globl call_table_and_ret
call_table_and_ret:
    blr x8
    mov x0, x25        // just tmp save
    msr daifset, #2
    str x25, [sp, #16] //     str   x0, [sp, #S_X0]             // returned x0
    kernel_exit

    //TODO error here 
    ret


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





//////////////////////////////////////////////////////////////////////////////////////////////

.macro save_uctx
	sub sp, sp, #8
	str x0, [sp]

	ldr x0, =LAYOUT_MY_CPU
	ldr x0, [x0]

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
	add sp, sp, #8 
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
	ldr x0, =LAYOUT_MY_CPU
	ldr x0, [x0]

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

.globl urestore_and_ret
restore_and_ret:
	restore_uctx
	eret

.macro save_kctx
.endm

.macro restore_kctx
// 288 offset of kctx inside task
	ldr x0, =LAYOUT_MY_CPU
	ldr x0, [x0]

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
