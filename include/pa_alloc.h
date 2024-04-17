#ifndef _PA_ALLOC_H
#define _PA_ALLOC_H 

#include "common.h"
#include "memlayout.h"

#define TERMINAL_PAGE MEM_START

struct run{
    struct run * next;
};

extern struct run * freepages;

void * memset(void *str, int c, size_t n);
void * memcpy(void *dest, const void * src, size_t n);

bool zero_range(char * astart, uint64_t size);
uint64_t init_pa_alloc(void);
uint64_t get_num_of_free_pages(void);
uint64_t get_page_unsafe(void);
uint64_t get_page(void);


bool free_page(uint64_t paddr);



#endif