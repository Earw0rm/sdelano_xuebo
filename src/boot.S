#include "arm/sysregs.h"
#include "pa_alloc.h"
.section ".text.boot"

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF // check processor id
    cbnz x0, _proc_hang

    add x1, x0, #1
    lsl x1, x1, #12

    adr x2, init_stack3
    add x1, x1, x2 
    mov sp, x1     

    adr x0, _bss_begin
    adr x1, _bss_end
_zero_bss:
    sub x2, x1, x0
    cbz x2, configure_el3
    str xzr,[x0]
    add x0, x0, #8
    b _zero_bss
    b _proc_hang // hang for all non-primary CPU

_proc_hang:
    wfi
    b _proc_hang



el1_entry:
// why do we need do this? coze of manual?
// ok now i know why. Free inside configure_el3
//    adr x0, bss_begin
//    adr x1, bss_end
//    bl zero_range 

    ldr x2, =vectors
    msr vbar_el1, x2
    bl kernel_main
    b _proc_hang
