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




#endif 

#endif