#include "pa_alloc.h"
#include "common.h"



struct run * freepages = ((struct run*) TERMINAL_PAGE);

// astart must be less then astop
bool zero_range(uint64_t * astart, uint64_t * astop){

    if(astart >= astop) return false;

    for(uint64_t * ptr = astart; ptr < astop; ++ptr){
        *ptr = 0;
    }

    return true;
}

uint64_t init_pa_alloc(void){
    //we have 2 ranges of memory. One befor VC sdram one after.
    uint64_t * sdram_range_1 = ((uint64_t *) SP_EL3_BOT_MASTER_GUARD_PAGE_STOP);
    uint64_t * sdram_range_1_end = ((uint64_t *) VC_BASE_TOP);

    uint64_t * sdram_range_2 = ((uint64_t *) VC_BASE_BOT);
    uint64_t * sdram_range_2_end = ((uint64_t *) KERNEL_GUARD_PAGE);

    uint64_t alloc_pages_counter = 0;
    for(;sdram_range_2_end < sdram_range_2; sdram_range_2_end += PAGE_SIZE){
        struct run * page = (struct run *) sdram_range_2_end;
        page->next = freepages;
        freepages = page;
        ++alloc_pages_counter;
    }
    for(;sdram_range_1_end < sdram_range_1;sdram_range_1_end += PAGE_SIZE){
        struct run * page = (struct run *) sdram_range_1_end;
        page->next = freepages;
        freepages = page;
        ++alloc_pages_counter;
    }


    return alloc_pages_counter;
}

uint64_t get_num_of_free_pages(void){
    struct run * terminal_page = ((struct run*) TERMINAL_PAGE); 
    uint64_t counter = 0;
    for(struct run * cpage = freepages;
        cpage != terminal_page; 
        cpage = cpage->next){
            ++counter;
        }
    return counter;
}