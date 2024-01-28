#include "mm.h"
#include "arm/sysregs.h"
#include "arm/cfg_el3.h"
.section ".text.boot"

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF // check processor id
    cbz x0, master // hang for all non-primary CPU
    b proc_hang

proc_hang:
    wfi
    b proc_hang


master:
    mov x1, #SP_EL1_MASTER_STACK 
    msr sp_el1, x1

    mov sp, #SP_EL3_MASTER_STACK  // sp_el3

    mov x1, #SP_EL0_MASTER_STACK // sp_el0
    msr sp_el0, x1

    bl configure_el3


    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0

    ldr x0, =HCR_VALUE
    msr hcr_el2, x0

    ldr x0, =SCR_VALUE
    msr scr_el3, x0

    ldr x0, =SPSR_VALUE
    msr spsr_el3, x0

    adr x0, el1_entry
    msr elr_el3, x0


    eret
    
el1_entry:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero 

    ldr x2, =vectors
    msr vbar_el1, x2
    
    bl kernel_main
    b proc_hang
