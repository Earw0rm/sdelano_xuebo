#ifndef _ARM_UTIL_H
#define _ARM_UTIL_H



#ifndef __ASSEMBLER__
#include "common.h"



static inline void disable_irq(void){
    asm volatile( "msr daifset, #2" ); 
}

static inline void enable_irq(void){
    asm volatile("msr daifclr, #2" ); 
}

static inline uint32_t get_current_el(void){
    uint32_t x;
    asm volatile("mrs %0, CurrentEL" : "=r" (x));
    return (x >> 2);
}

static inline uint64_t get_exception_syndrome1(void){
    uint64_t x;
    asm volatile("mrs %0, esr_el1" : "=r" (x));
    return x;
}

static inline uint64_t get_syscall_number(void){
    uint64_t x;
    asm volatile("mov %0, w8" : "=r" (x));
    return x;
}

static inline void w_sctlr_el1(uint64_t x){
    asm volatile("msr sctlr_el1, %0" : : "r" (x));
}

static inline void w_hcr_el2(uint64_t x){
    asm volatile("msr hcr_el2, %0" : : "r" (x));
}

static inline void w_scr_el3(uint64_t x){
    asm volatile("msr scr_el3, %0" : : "r" (x));
}
static inline void w_spsr_el3(uint64_t x){
    asm volatile("msr spsr_el3, %0" : : "r" (x));
}

static inline void w_vbar_el1(uint64_t x){
    asm volatile("msr vbar_el1, %0" : : "r" (x));
}
static inline void w_elr_el3(uint64_t x){
    asm volatile("msr elr_el3, %0" : : "r" (x));
}

#endif 

#endif