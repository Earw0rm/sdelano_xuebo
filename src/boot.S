.section ".data.boot"
_bss_is_clear:
    .quad 0x0

.section ".text.boot"
.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF // check processor id
    
    // set stack (init_stack3 in start.c)
    add x1, x0, #1
    lsl x1, x1, #12

    ldr x2, =init_stack3 //in start.c
    add x1, x1, x2 
    mov sp, x1

    // if it is not core0 then wait
    cbnz x0, _wait_for_bss_clear

    // set address range for clear
    ldr x0, =_bss_begin
    ldr x1, =_bss_end

_zero_bss:
    sub x2, x1, x0
    cbz x2, __break_zero_bss
    str xzr, [x0]
    add x0, x0, #8
    b _zero_bss
__break_zero_bss:
// there is dependency between row 3 and row 2. Barrier does not need
    mov x1, #1
    ldr x5, =_bss_is_clear
    str x1, [x5]
//    dmb st
    b _prepare_id_and_configure



_wait_for_bss_clear:
// there is dependency in 1st 3 row. Barrier does not need
    ldr x5, =_bss_is_clear
    ldr x6, [x5] 
    cbz x6, _wait_for_bss_clear
    b _prepare_id_and_configure
    b _proc_hang // should not be achievable

_prepare_id_and_configure:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF // check processor id
    b configure_el3
    b _proc_hang // should not be achievable


_proc_hang:
    wfi
    b _proc_hang
