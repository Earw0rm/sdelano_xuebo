#ifndef BASEH
#define BASEH

#include "vm.h"

#define PBASE (KERNEL_VA_BASE + 0xFE000000)
//  0xff840000
#define GIC400_BASE (PBASE + 0x1840000) 
// 0xfe003000
#define SYS_TIMER_BASE (PBASE + 0x00003000)

#endif