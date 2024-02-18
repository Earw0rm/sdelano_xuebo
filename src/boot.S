#include "arm/sysregs.h"
#include "pa_alloc.h"
.section ".text.boot"

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF // check processor id
    
    add x1, x0, #1
    lsl x1, x1, #12

    adr x2, init_stack3
    add x1, x1, x2 
    mov sp, x1     

    cbz x0, configure_el3 // hang for all non-primary CPU
    b proc_hang

proc_hang:
    wfi
    b proc_hang
    
el1_entry:
// why do we need do this? coze of manual?
//    adr x0, bss_begin
//    adr x1, bss_end
//    bl zero_range 

    ldr x2, =vectors
    msr vbar_el1, x2
    bl kernel_main
    b proc_hang
