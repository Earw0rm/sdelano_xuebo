

.globl fork_ret
fork_ret:
    bl preempt_enable
    mov x0, x20
    blr x19 // should never return
