#ifndef BASEH
#define BASEH

#include "vm.h"
#include "common.h"

extern bool configuration_is_completed;

#define PBASE (0xFE000000ull + configuration_is_completed * (0xffffull << 48))
//  0xff840000
#define GIC400_BASE (PBASE + 0x1840000) 
// 0xfe003000
#define SYS_TIMER_BASE (PBASE + 0x00003000)


#endif