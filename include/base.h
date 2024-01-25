#ifndef BASEH
#define BASEH

// periphiral base lss
#if RPI_VERSION == 3
#define PBASE 0x3F000000

#elif RPI_VERSION == 4
#define PBASE 0xFE000000

//  0xff840000
#define GIC400_BASE (PBASE + 0x1840000) 

// 0xfe003000
#define SYS_TIMER_BASE (PBASE + 0x00003000)
#else
#define PBASE 0 
#error PRI_VERSION NOT DEFINED


#endif 
#endif