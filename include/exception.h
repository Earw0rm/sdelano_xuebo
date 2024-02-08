#ifndef _EXCEPTION_ENTRY_H
#define _EXCEPTION_ENTRY_H



// ***************************************
// Exceptiom vector table, Page 5357 of AArch64-Reference-Manual (A-profile architecture reference manual).    
// ***************************************

#define S_FRAME_SIZE 256 // stack frame size (maximum)

#define SYNC_INVALID_EL1t    0 
#define IRQ_INVALID_EL1t     1 
#define FIQ_INVALID_EL1t     2 
#define ERROR_INVALID_EL1t   3 

#define SYNC_INVALID_EL1h    4  
#define IRQ_INVALID_EL1h     5 
#define FIQ_INVALID_EL1h     6 
#define ERROR_INVALID_EL1h   7 

#define SYNC_INVALID_EL0_64  8 
#define IRQ_INVALID_EL0_64   9 
#define FIQ_INVALID_EL0_64   10 
#define ERROR_INVALID_EL0_64 11 

#define SYNC_INVALID_EL0_32  12 
#define IRQ_INVALID_EL0_32   13 
#define FIQ_INVALID_EL0_32   14 
#define ERROR_INVALID_EL0_32 15 

#ifndef __ASSEMBLER__
#include "common.h"
void show_invalid_entry_message(uint32_t, uint32_t, uint32_t);
void handle_irq(void);
void handle_el0_sync(uint64_t, uint64_t,
                     uint64_t, uint64_t,
                     uint64_t, uint64_t,
                     uint64_t, uint64_t,
                     uint64_t);
                     
void* call_table_and_ret(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, 
                         uint64_t x5, uint64_t x6, uint64_t x7, uint64_t fn_addr);
#endif


#endif 