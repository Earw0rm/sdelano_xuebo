#ifndef _PA_ALLOC_H
#define _PA_ALLOC_H 

#include "common.h"



#define PAGE_SIZE 4096 // bytes

#define ADDR_ROUND_DOWN(x) (((uint64_t)(x)) & (~0xfff))
#define PAGE_UP(x) ((((uint64_t)(x)) & (~0xfff)) + (0x1000))
#define TOP_OF_THE_PAGE(x) ((((uint64_t)x) & (~0xfff)) + (0x1000 - 0x0010))


// #define TOP_OF_THE_PAGE(x) ((((uint64_t)x) & (~0xfff)) + (0x1000 - 0x0004))
// legacy master view of physical memory
#define PASTOP        (0x1f9999999) // 8gb rpi 
#define SDRAM_BOT     (0x100000000)
#define SDRAM_TOP     (0xFC000000)
#define VC_BASE_TOP   (0x40000000)
//todo set variable from makefile and config.txt
#define VC_BASE_BOT   (VC_BASE_TOP - 0x10000000) // 256 mb vc sdram


#define PA_KERNEL_END (&_end_of_kernel_addr)
#define TERMINAL_PAGE (PA_KERNEL_END + 1*PAGE_SIZE)
#define KERNEL_GUARD_PAGE (TERMINAL_PAGE)



#ifndef __ASSEMBLER__
extern volatile const char _end_of_kernel_addr;


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