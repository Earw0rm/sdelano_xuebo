#include "pa_alloc.h"
#include "common.h"
#include "printf.h"

// each page it is pointer to LOWER address.
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
    //TODO there's a bug here. The number of pages returned from the method is 1 more than it should be

    
    // ranges of memory
    // KERNEL_GUARD_PAGE => VC_BASE_BOT; VC_BASE_TOP => SDRAM_TOP; SDRAM_BOT => PASTOP 
    // important point that page is pointer to bottom address
    // then, for example if we need to allocate page for stack, we need to get top address of the page

    uint64_t alloc_pages_counter = 0;

    uint64_t * sdram_range_1 = ((uint64_t *) KERNEL_GUARD_PAGE);
    uint64_t * sdram_range_1_end = ((uint64_t *) VC_BASE_BOT);

    uint64_t * sdram_range_2 = ((uint64_t *) VC_BASE_TOP);
    uint64_t * sdram_range_2_end = ((uint64_t *) SDRAM_TOP);

    uint64_t * sdram_range_3 = ((uint64_t *) SDRAM_BOT);
    uint64_t * sdram_range_3_end = ((uint64_t *) PASTOP);


    printf("Ranges of pa_alloc %X => %X, %X => %X, %X => %X \r\n",
     KERNEL_GUARD_PAGE,
     VC_BASE_BOT,
     VC_BASE_TOP,
     SDRAM_TOP,
     SDRAM_BOT,
     PASTOP);


    for(;sdram_range_1 < sdram_range_1_end; sdram_range_1 += PAGE_SIZE){
        struct run * page = (struct run *) sdram_range_1;
        page->next = freepages;
        freepages = page;
        ++alloc_pages_counter;
//        if(alloc_pages_counter % 100000 == 0) printf("Init address of pa1 = %x \r\n", sdram_range_1);
    }

    for(;sdram_range_2 < sdram_range_2_end; sdram_range_2 += PAGE_SIZE){
        struct run * page = (struct run *) sdram_range_2;
        page->next = freepages;
        freepages = page;
        ++alloc_pages_counter;
//        if(alloc_pages_counter % 100000 == 0) printf("Init address of pa2 = %x \r\n", sdram_range_2);
    }
    for(;sdram_range_3 < sdram_range_3_end; sdram_range_3 += PAGE_SIZE){
        struct run * page = (struct run *) sdram_range_3;
        page->next = freepages;
        freepages = page;
        ++alloc_pages_counter;
//        if(alloc_pages_counter % 100000 == 0) printf("Init address of pa3 = %x \r\n", sdram_range_3);
    }
//    printf("Ret \r\n");
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

uint64_t get_page(void){
    struct run * page = freepages;
    if(page == ((struct run*) TERMINAL_PAGE)){
        return 0;
    }
    freepages = page->next;
    return ((uint64_t) page);
}

bool free_page(uint64_t paddr){
    if(paddr == 0){
        return false;
    }        
    struct run * page = ((struct run *) paddr);
    page->next = freepages;
    freepages = page;
    return true;
}