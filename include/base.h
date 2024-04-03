#ifndef BASEH
#define BASEH

#include "vm.h"
#include "common.h"

extern bool configuration_is_completed;



#define MAIN_PERIPHERAL_TOP      (0x0ff800000ull)
#define MAIN_PERIPHERAL_BOT      (0x0fc000000ull)
#define ARM_LOCAL_PERIPHERAL_TOP (0x100000000ull)
#define ARM_LOCAL_PERIPHERAL_BOT (MAIN_PERIPHERAL_TOP)

// peripheral can be acquired only throo mmu
#define PBASE ((MAIN_PERIPHERAL_BOT + 0x2000000) + (0xffffull << 48)) 
//  0xff840000
#define GIC400_BASE (PBASE + 0x1840000) 
// 0xfe003000
#define SYS_TIMER_BASE (PBASE + 0x00003000)

#define TMPPBASE ((MAIN_PERIPHERAL_BOT + 0x2000000))

#endif