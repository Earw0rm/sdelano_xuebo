#include "vm.h"
#include "pa_alloc.h"
#include "arm/arm_util.h"
#include "printf.h"



/**
 * TLDR: 
 * The page table address must be aligned to the page size. For example, for 4kb the lower 12 bits must be zeros.
 * Then in ttbr we write only bits of our physical address starting from 47:12, the rest are implied by zeros.
*/
// The number of OA bits held in TTBR_ELx is determined by the granule size and OA address size. The bits used
// for each granule size when using the maximum OA address size of 48 bits or 52 bits are shown in the following
// table. Software might be required to set one or more of the low-order base address bits to zero to align the table to
// the table size.
// Granule size       | Maximum OA size    | Translation table base address | OA bits held in TTBR_ELx
// 4KB, 16KB, or 64KB | 52 bits            | TTBR_ELx[5:2]                  | OA[51:48]
// 4KB                | 48 bits or 52 bits | TTBR_ELx[47:12]                | OA[47:12]
// 16KB               | 48 bits or 52 bits | TTBR_ELx[47:14]                | OA[47:14]
// 64KB               | 48 bits or 52 bits | TTBR_ELx[47:16]                | OA[47:16]
// __attribute__((aligned(16))) volatile char kpgtbl[4096 * 4] = {0};

__attribute__((aligned(0x1000))) volatile char kpgtbl[0x4000] = {0};

// Return the address of the PTE in page table pagetable
// that corresponds to virtual address va.  If alloc!=0,
// create any required page-table pages.
pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc){
    // if va >= maxva then panic 

    for(uint64_t level = 0; level <= 2; ++level){


        uint64_t pgtbl_index = VA_PTBL_IND(va, level);
        pte_t *pte = &pagetable[pgtbl_index];

        if(((*pte) & VALID_DESCRIPTOR) == 1){ // mean that PTE is valid
            pagetable = (pagetable_t)DAADDR((uint64_t)*pte);

        }else{
            if(!alloc) return 0;
            uint64_t page = get_page();

            zero_range((uint64_t *) page, PAGE_SIZE);
            
            if((page & 0xfffull) != 0) return 0; // mean that they are not page aligned

            *pte = (page | TABLE_DESCRIPTOR | VALID_DESCRIPTOR);
            pagetable = (pagetable_t)page;
        }

    }


    return &pagetable[VA_PTBL_IND(va, 3)]; // last 3nd level without offset
}


// address round down before map. Support only per page mapping
uint64_t mapva(uint64_t va, uint64_t pa, pagetable_t pgtbl, mair_ind ind){
    va = PGROUNDDOWN(va);
    pa = PGROUNDDOWN(pa);

    if((va & 0xfffull) != 0 || (pa & 0xfffull) != 0) return -1;

    pte_t* pte =  walk(pgtbl, va, true);
    if(pte == 0){
        return -1;
    }
    

    // CHECK PAGE LOWER ATTRIBUTES AND UPPER ATTRIBUTES
    uint64_t page_str = (pa | VALID_DESCRIPTOR | PAGE_DESCRIPTOR | (ind << 2) | ACCESS_FLAG | ACCESS_PERMISSION);

    (*pte) = page_str;
    return 0;
}
/**
 * 1) Мапим все ядро от kernel_end до 0 как mair_3 non_sharable
 * 2) Мапим локи как mair_2 inner_sharable
 * 3) Мапим девайсы как device_ngrne
*/
pagetable_t init_mmu(uint64_t core_id){
    pagetable_t pgtbl = (pagetable_t) &kpgtbl[core_id * 4096];


    // general all kernel code and variables
    for(char * pointer = 0; pointer < PA_KERNEL_END; pointer += 0x1000){
        uint64_t res = mapva((VAKERN_BASE | ((uint64_t) pointer)), (uint64_t) pointer, pgtbl, NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT);
        if(res < 0) return 0;
    }
    // shared kernel 
    for(char * pointer = ((char *) PA_THREAD_SHARED_DATA_BEGIN); pointer < ((char *) PA_THREAD_SHARED_DATA_END); pointer += 0x1000){
        uint64_t res = mapva((VAKERN_BASE | ((uint64_t) pointer)), (uint64_t) pointer, pgtbl, NORMAL_NC);
        if(res < 0) return 0;
    }
    //devices
    //TODO not implemented
    
    return pgtbl;
}


