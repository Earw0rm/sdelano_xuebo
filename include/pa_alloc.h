#ifndef _PA_ALLOC_H
#define _PA_ALLOC_H 

#include "common.h"



#define PAGE_SIZE 4096 // bytes
// legacy master view of physical memory
#define PASTOP (0x400000000)
#define SDRAM_BOT_2 (0x100000000)
#define SDRAM_TOP (0xFC000000)
#define VC_BASE_TOP   (0x40000000)
#define VC_BASE_BOT (VC_BASE_END - 0x10000000) // 256 mb vc sdram

#define PA_KERNEL_END (&_end_of_kernel_addr)


// 3 стэка каждый по одной страничке начиная с PA_KERNEL_END
#define SP_EL0_MASTER_STACK (SDRAM_TOP - 1*PAGE_SIZE)
#define SP_EL1_MASTER_STACK (SDRAM_TOP - 2*PAGE_SIZE)
#define SP_EL3_MASTER_STACK (SDRAM_TOP - 3*PAGE_SIZE)

#ifndef __ASSEMBLER__
extern const int _end_of_kernel_addr;


struct run{
    struct run * next;
};

extern struct run * freepages;


bool zero_range(uint64_t * astart, uint64_t * astop);

// void free_page(uint64_t p);
// uint64_t get_free_page(void);

#endif 

#endif 