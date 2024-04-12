
.macro save_kctx kctx_addr
	mov x1, \rfrom_addr

	stp	x19, x20, [x1, #16 * 0]
	stp	x21, x22, [x1, #16 * 1]
	stp	x23, x24, [x0, #16 * 2]
	stp	x25, x26, [x0, #16 * 3]
	stp	x27, x28, [x0, #16 * 4]
	stp	x29, x30, [x0, #16 * 5]

	mrs x3, sp_el1 
	str x3, [x0, #96]

	mrs x3, ttbr0_el1 
	str x3, [x0, #104]

	mrs x3, ttbr1_el1 
	str x3, [x0, #112]
.endm

.macro restore_kctx kctr_addr
	mov x1, \rfrom_addr

	ldp	x19, x20, [x1, #16 * 0]
	ldp	x21, x22, [x1, #16 * 1]
	ldp	x23, x24, [x0, #16 * 2]
	ldp	x25, x26, [x0, #16 * 3]
	ldp	x27, x28, [x0, #16 * 4]
	ldp	x29, x30, [x0, #16 * 5]

	ldr x3, [x0, #96]
	msr sp_el1, x3 

	ldr x3, [x0, #104]
	msr ttbr0_el1, x3

	ldr x3, [x0, #112]
	msr ttbr1_el1, x3

//	isb
//	ret 
.endm

.globl
_switch_to:
	stp	x19, x20, [x0, #16 * 0]
	stp	x21, x22, [x0, #16 * 1]
	stp	x23, x24, [x0, #16 * 2]
	stp	x25, x26, [x0, #16 * 3]
	stp	x27, x28, [x0, #16 * 4]
	stp	x29, x30, [x0, #16 * 5]

	mrs x3, sp_el1 
	str x3, [x0, #96]

	mrs x3, ttbr0_el1 
	str x3, [x0, #104]

	mrs x3, ttbr1_el1 
	str x3, [x0, #112]


	ldp	x19, x20, [x1, #16 * 0]
	ldp	x21, x22, [x1, #16 * 1]
	ldp	x23, x24, [x1, #16 * 2]
	ldp	x25, x26, [x1, #16 * 3]
	ldp	x27, x28, [x1, #16 * 4]
	ldp	x29, x30, [x1, #16 * 5]

	ldr x3, [x1, #96]
	msr sp_el1, x3 

	ldr x3, [x1, #104]
	msr ttbr0_el1, x3

	ldr x3, [x1, #112]
	msr ttbr1_el1, x3

	isb
	ret 
