#ifndef _PA_ALLOC_H
#define _PA_ALLOC_H 

#include "common.h"



#define PAGE_SIZE 4096 // bytes

#define ADDR_ROUND_DOWN(x) (((uint64_t)(x)) & (~0xfff))
#define PAGE_UP(x) ((((uint64_t)(x)) & (~0xfff)) + (0x1000))
#define TOP_OF_THE_PAGE(x) ((((uint64_t)x) & (~0xfff)) + (0x1000 - 0x0010))


// #define TOP_OF_THE_PAGE(x) ((((uint64_t)x) & (~0xfff)) + (0x1000 - 0x0004))
// legacy master view of physical memory
#define PASTOP (0x400000000)
#define SDRAM_BOT_2 (0x100000000)
#define SDRAM_TOP (0xFC000000)
#define VC_BASE_TOP   (0x40000000)
#define VC_BASE_BOT (VC_BASE_TOP - 0x10000000) // 256 mb vc sdram


#define PA_KERNEL_END (&_end_of_kernel_addr)
#define TERMINAL_PAGE (PA_KERNEL_END + 1*PAGE_SIZE)

#define KERNEL_GUARD_PAGE (TERMINAL_PAGE)


#define MASTER_STACK (KERNEL_GUARD_PAGE + PAGE_SIZE)

// 3 base master stack and guard pages
#define SP_EL0_MASTER_GUARD_PAGE_START (SDRAM_TOP - 1*PAGE_SIZE)
#define SP_EL0_MASTER_GUARD_PAGE_STOP (SDRAM_TOP - 2*PAGE_SIZE)

#define SP_EL0_MASTER (SP_EL0_MASTER_GUARD_PAGE_STOP)

#define SP_EL1_MASTER_GUARD_PAGE_START (SP_EL0_MASTER - 3*PAGE_SIZE)
#define SP_EL1_MASTER_GUARD_PAGE_STOP (SP_EL1_MASTER_GUARD_PAGE_START - 1*PAGE_SIZE)

#define SP_EL1_MASTER (SP_EL1_MASTER_GUARD_PAGE_STOP)

#define SP_EL3_MASTER_GUARD_PAGE_START (SP_EL1_MASTER - 3*PAGE_SIZE)
#define SP_EL3_MASTER_GUARD_PAGE_STOP (SP_EL3_MASTER_GUARD_PAGE_START - 1*PAGE_SIZE)

#define SP_EL3_MASTER (SP_EL3_MASTER_GUARD_PAGE_STOP)

#define SP_EL3_BOT_MASTER_GUARD_PAGE_START (SP_EL3_MASTER - 3*PAGE_SIZE)
#define SP_EL3_BOT_MASTER_GUARD_PAGE_STOP (SP_EL3_BOT_MASTER_GUARD_PAGE_START - 1*PAGE_SIZE)






#ifndef __ASSEMBLER__
extern const int _end_of_kernel_addr;


struct run{
    struct run * next;
};

extern struct run * freepages;


bool zero_range(uint64_t * astart, uint64_t * astop);
uint64_t init_pa_alloc(void);
uint64_t get_num_of_free_pages(void);

uint64_t get_page(void);

bool free_page(uint64_t paddr);
// void free_page(uint64_t p);
// uint64_t get_free_page(void);

#endif 

#endif 