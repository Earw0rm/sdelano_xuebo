.globl fork_ret
fork_ret:
    bl dump3
    bl preempt_enable
    mov x0, x20
    blr x19 // should never return
