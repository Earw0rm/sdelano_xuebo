#include "sched.h"


// uint64_t x29_fp; // frame pointer
// uint64_t x30_lr; // link register
// uint64_t x31_sp; // stack pointer for current thread    
.globl cpu_switch_to
cpu_switch_to: // void cpu_switch_to(prev, next);
    mov x10, #CPU_CONTEXT_OFFSET
    add x8, x0, x10
    mov x9, sp
    stp x19, x20, [x8], #16 //store callee-saved registers
    stp x21, x22, [x8], #16
    stp x23, x24, [x8], #16
    stp x25, x26, [x8], #16
    stp x27, x28, [x8], #16
    stp x29, x9,  [x8], #16
    str x30,      [x8]

    add x8, x1, x10
    ldp x19, x20, [x8], #16 // restore callee-saved registers
    ldp x21, x22, [x8], #16
    ldp x23, x24, [x8], #16
    ldp x25, x26, [x8], #16
    ldp x27, x28, [x8], #16
    ldp x29, x9,  [x8], #16
    ldr x30,      [x8]
    mov sp,  x9 
    
    
    adr x0, fork_ret
    mov x1, x30 // какого то хуя тут топ оф зе паге (понятно почему просто переложи и сделай нормально)
    bl debug_addr
    mov x6, x30

    ret // this is where magic happens

    