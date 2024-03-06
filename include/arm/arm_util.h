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

static inline uint64_t r_sctlr_el1(void){
    uint64_t x;
    asm volatile("mrs %0, sctlr_el1" : "=r" (x));
    return x;
}

static inline void w_hcr_el2(uint64_t x){
    asm volatile("msr hcr_el2, %0" : : "r" (x));
}

static inline uint64_t r_hcr_el2(void){
    uint64_t x;
    asm volatile("mrs %0, hcr_el2" : "=r" (x));
    return x;
}


static inline void w_scr_el3(uint64_t x){
    asm volatile("msr scr_el3, %0" : : "r" (x));
}

static inline uint64_t r_scr_el3(void){
    uint64_t x;
    asm volatile("mrs %0, scr_el3" : "=r" (x));
    return x;
}

static inline void w_spsr_el3(uint64_t x){
    asm volatile("msr spsr_el3, %0" : : "r" (x));
}

static inline uint64_t r_spsr_el3(void){
    uint64_t x;
    asm volatile("mrs %0, spsr_el3" : "=r" (x));
    return x;
}

static inline void w_vbar_el1(uint64_t x){
    asm volatile("msr vbar_el1, %0" : : "r" (x));
}

static inline uint64_t r_vbar_el1(void){
    uint64_t x;
    asm volatile("mrs %0, vbar_el1" : "=r" (x));
    return x;
}

static inline void w_elr_el3(uint64_t x){
    asm volatile("msr elr_el3, %0" : : "r" (x));
}

static inline uint64_t r_elr_el3(void){
    uint64_t x;
    asm volatile("mrs %0, elr_el3" : "=r" (x));
    return x;
}


static inline void w_vbar_el3(uint64_t x){
    asm volatile("msr vbar_el3, %0" : : "r" (x));
}

static inline uint64_t r_vbar_el3(void){
    uint64_t x;
    asm volatile("mrs %0, vbar_el3" : "=r" (x));
    return x;
}

static inline void w_sp_el0(uint64_t x){
    asm volatile("msr sp_el0, %0" : : "r" (x));
}

static inline uint64_t r_sp_el0(void){
    uint64_t x;
    asm volatile("mrs %0, sp_el0" : "=r" (x));
    return x;
}

static inline void w_sp_el1(uint64_t x){
    asm volatile("msr sp_el1, %0" : : "r" (x));
}

static inline uint64_t r_sp_el1(void){
    uint64_t x;
    asm volatile("mrs %0, sp_el1" : "=r" (x));
    return x;
}

static inline void enable_mmu(void) {
    asm volatile(
        "mrs x0, sctlr_el1\n"
        "orr x0, x0, #(1 << 0)\n"
        "msr sctlr_el1, x0\n"
        "isb\n"
        :
        :
        : "x0", "memory"
    );
}

// MMU part 
static inline void w_tcr_el1(uint64_t x){
    asm volatile("msr tcr_el1, %0" : : "r" (x));
}

static inline uint64_t r_tcr_el1(void){
    uint64_t x;
    asm volatile("mrs %0, tcr_el1" : "=r" (x));
    return x;
}

static inline void w_mair_el1(uint64_t x){
    asm volatile("msr mair_el1, %0" : : "r" (x));
}

static inline uint64_t r_mair_el1(void){
    uint64_t x;
    asm volatile("mrs %0, mair_el1" : "=r" (x));
    return x;
}

static inline void w_ttbr0_el1(uint64_t x){
    asm volatile("msr ttbr0_el1, %0" : : "r" (x));
}

static inline uint64_t r_ttbr0_el1(void){
    uint64_t x;
    asm volatile("mrs %0, ttbr0_el1" : "=r" (x));
    return x;
}

static inline void w_ttbr1_el1(uint64_t x){
    asm volatile("msr ttbr1_el1, %0" : : "r" (x));
}

static inline uint64_t r_ttbr1_el1(void){
    uint64_t x;
    asm volatile("mrs %0, ttbr1_el1" : "=r" (x));
    return x;
}

static inline uint64_t r_mpidr_el1(void){
    uint64_t x;
    asm volatile("mrs %0, mpidr_el1" : "=r" (x));
    return x;
}

static inline uint64_t r_id_aa64mmfr0_el1(void){
    uint64_t x;
    asm volatile("mrs %0, id_aa64mmfr0_el1" : "=r" (x));
    return x;
}
/*
* ID_AA64MMFR0_EL1.PARange PA memory size PA address size
* 0b0000 4GB 32 bits,   PA[31:0]
* 0b0001 64GB 36 bits,  PA[35:0]
* 0b0010 1TB 40 bits,   PA[39:0]
* 0b0011 4TB 42 bits,   PA[41:0]
* 0b0100 16TB 44 bits,  PA[43:0]
* 0b0101 256TB 48 bits, PA[47:0]
* 0b0110 4PB 52 bits,   PA[51:0]
*/
static inline uint64_t get_parange(void){
    return (r_id_aa64mmfr0_el1() & 0xf);
}

static inline uint64_t get_processor_id(void){
    return (r_mpidr_el1() & 0xff);
}


#endif 
#endif