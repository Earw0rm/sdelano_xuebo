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



// legacy master view
#define PASTOP (0x400000000)
#define SDRAM_BOT_2 (0x100000000)
#define SDRAM_TOP (0xFC000000)
#define VC_BASE_TOP   (0x40000000)
#define VC_BASE_BOT (VC_BASE_END - 0x10000000) // 256 mb vc sdram
#define PA_KERNEL_END (&_end_of_kernel_addr)

#ifndef __ASSEMBLER__
extern const int _end_of_kernel_addr;
#endif

#else
#define PBASE 0 
#error PRI_VERSION NOT DEFINED


#endif 
#endif