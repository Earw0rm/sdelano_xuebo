#include "vm.h"
#include "pa_alloc.h"
#include "arm/arm_util.h"


__attribute__((aligned(16))) volatile char kpgtbl[4096 * 4] = {0};

// Return the address of the PTE in page table pagetable
// that corresponds to virtual address va.  If alloc!=0,
// create any required page-table pages.
pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc){
    // if va >= maxva then panic 

    for(uint64_t level = 0; level <= 2; ++level){

        uint64_t pgtbl_index = VA_PTBL_IND(va, level);
        pte_t *pte = &pagetable[pgtbl_index];

        if(((*pte) & VALID_DESCRIPTOR) == 1){ // mean that PTE is valid
            pagetable = (pagetable_t)PTE2PA(pte);
        }else{
            if(!alloc) return 0;
            uint64_t page = get_page();
            zero_range((uint64_t *) page, PAGE_SIZE);

            *pte = (PA2PTE(page) | TABLE_DESCRIPTOR | VALID_DESCRIPTOR);
            pagetable = (pagetable_t)PTE2PA(pte);
        }

    }

    return &pagetable[VA_PTBL_IND(va, 3)]; // last 3nd level without offset
}


// address round down before map. Support only per page mapping
uint64_t mapva(uint64_t va, uint64_t pa, pagetable_t pgtbl, mair_ind ind){
    va = PGROUNDDOWN(va);
    pa = PGROUNDDOWN(pa);
    pte_t* pte =  walk(pgtbl, va, true);
    if(pte == 0){
        return -1;
    }
    // CHECK PAGE LOWER ATTRIBUTES AND UPPER ATTRIBUTES
    uint64_t page_str = (PA2PTE(pa) | VALID_DESCRIPTOR | PAGE_DESCRIPTOR | (ind << 2));
    *pte = page_str;
    return 0;
}

pagetable_t init_mmu(void){
    pagetable_t pgtbl = (pagetable_t) &kpgtbl[get_processor_id() * 4096];
    
    for(char * pointer = 0; pointer < PA_KERNEL_END; pointer += 4096){
        uint64_t res = /**VAKERN_BASE |*/ mapva(( ((uint64_t) pointer)), (uint64_t) pointer, pgtbl, NORMAL_NC);
        if(res < 0) return 0;
    }
    return pgtbl;
}


