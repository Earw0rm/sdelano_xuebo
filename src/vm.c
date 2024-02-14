#include "vm.h"

pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc){
    // if va >= maxva then panic 

    for(uint64_t level = 0; level <= 3; ++level){

        pte_t *pte = &pagetable[VA_PTBL_IND(level, va)];
        if((*pte & 1) == 1){ // mean that PTE is valid
            
            pagetable = (pagetable_t)PTE2PA(pte);
        }else{
            // we can create new entry if alloc = true. if not just return 0 as pointer to pte
        }

    }
}