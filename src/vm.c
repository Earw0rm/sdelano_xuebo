#include "vm.h"
#include "pa_alloc.h"


// Return the address of the PTE in page table pagetable
// that corresponds to virtual address va.  If alloc!=0,
// create any required page-table pages.
pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc){
    // if va >= maxva then panic 

    for(uint64_t level = 0; level <= 2; ++level){

        pte_t *pte = &pagetable[VA_PTBL_IND(level, va)];
        if((*pte & VALID_DESCRIPTOR) == 1){ // mean that PTE is valid
            pagetable = (pagetable_t)PTE2PA(pte);
        }else{
            if(!alloc) return 0;
            uint64_t page = get_page();
            zero_range((uint64_t *) page, (uint64_t *) PAGE_UP(page));
            *pte = (PA2PTE(page) | VALID_DESCRIPTOR);
        }

    }

    return &pagetable[VA_PTBL_IND(3, va)]; // last 3nd level without offset
}