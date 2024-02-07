#ifndef _ARM_UTIL_H
#define _ARM_UTIL_H



#ifndef __ASSEMBLER__
#include "common.h"



static inline void disable_irq(){
    asm volatile( "msr daifset, #2" ); 
}

static inline void enable_irq(){
    asm volatile("msr daifclr, #2" ); 
}

static inline uint32_t get_current_el(){
    uint32_t x;
    asm volatile("mrs %0, CurrentEL" : "=r" (x));
    return (x >> 2);
}

static inline uint64_t get_exception_syndrome1(){
    uint64_t x;
    asm volatile("mrs %0, esr_el1" : "=r" (x));
    return x;
}

static inline uint64_t get_syscall_number(){
    uint64_t x;
    asm volatile("mov %0, w8" : "=r" (x));
    return x;
}
#endif 

#endif