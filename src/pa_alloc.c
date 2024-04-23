#include "pa_alloc.h"
#include "common.h"
#include "printf.h"
#include "speenlock.h"
#include "base.h"
#include "memlayout.h"

__attribute__((section(".thread_shared")))
static struct speenlock pa_alloc_lock = {
    .cpu_num = -1,
    .locked = 0,
    .name = "clear"
};


// each page it is pointer to LOWER address.
struct run * freepages = ((struct run*) TERMINAL_PAGE);



void * memset(void *str, int c, size_t n){
    void * str_cpy = str;
    for(size_t i = 0; i < n; ++i){
        *((char *)str_cpy) = (char) c;
        str_cpy = (void *)(((char *)str_cpy) + 1);
    }
    return str;
}

//copies n characters from memory area src to memory area dest.
void * memcpy(void * dest, const void * src, size_t n){
    char * dest_cpy = (char *) dest;
    char * src_cpy  = (char *) src;
    for(size_t i = 0; i < n; ++i, ++dest_cpy, ++src_cpy){
        *dest_cpy = *src_cpy;   
    }
    return dest;
}


//size in word. crunch
bool zero_range(char * astart, uint64_t size){
    if(size == 0) return false;
    memset((void *) astart, 0, (size_t) size);
    return true;
}

uint64_t init_pa_alloc(void){
    //TODO there's a bug here. The number of pages returned from the method is 1 more than it should be

    
    // ranges of memory
    // KERNEL_GUARD_PAGE => VC_BASE_BOT; VC_BASE_TOP => SDRAM_TOP; SDRAM_BOT => PASTOP 
    // important point that page is pointer to bottom address
    // then, for example if we need to allocate page for stack, we need to get top address of the page

    uint64_t alloc_pages_counter = 0;


    for(char * pointer = (char *) MEM_TMP_START; pointer < (char *) MEM_TMP_END; pointer += 0x1000){
        struct run * page = (struct run *) pointer;
        page->next = freepages;
        freepages = page;
        ++alloc_pages_counter;
    }

    // for(char * pointer = (char *) MEM_VC_BASE_TOP; pointer < ((char *) MEM_KERN_TEXT_START); pointer += 0x1000){
    //     struct run * page = (struct run *) pointer;
    //     page->next = freepages;
    //     freepages = page;
    //     ++alloc_pages_counter;
    // }

    // for(char * pointer = (char *) MEM_KERN_END; pointer < ((char *) MEM_SDRAM_TOP); pointer += 0x1000){
    //     struct run * page = (struct run *) pointer;
    //     page->next = freepages;
    //     freepages = page;
    //     ++alloc_pages_counter;
    // }


    // for(char * pointer = (char *) MEM_SDRAM_BOT; pointer < ((char *) MEM_PASTOP); pointer += 0x1000){
    //     struct run * page = (struct run *) pointer;
    //     page->next = freepages;
    //     freepages = page;
    //     ++alloc_pages_counter;
    // }


    return alloc_pages_counter;
}

uint64_t get_num_of_free_pages(void){

    uint64_t counter = 0;
    for(struct run * cpage = freepages; cpage != ((struct run*) TERMINAL_PAGE); cpage = cpage->next){
            ++counter;

            if((((uint64_t)(&cpage)) & 0xfffull) != 0){
                return counter; // mean that some page are not 4kb aligment
            }
        }
     
    return counter;
}

uint64_t get_page_unsafe(void){
    struct run * page = freepages;
    if(page == ((struct run*) TERMINAL_PAGE)){
        return 0;
    }
    freepages = page->next;
    return ((uint64_t) page);    
}

uint64_t get_page(void){
    acquire(&pa_alloc_lock);
        struct run * page = freepages;
        
        if(page == ((struct run*) TERMINAL_PAGE)){
            release(&pa_alloc_lock);
            return 0;
        }

        freepages = page->next;
    release(&pa_alloc_lock);

    return ((uint64_t) page);

}


bool free_page(uint64_t paddr){

    if(paddr == 0){
        return false;
    }        

    release(&pa_alloc_lock);
        struct run * page = ((struct run *) paddr);
        page->next = freepages;
        freepages = page;
    release(&pa_alloc_lock);

    return true;

}

