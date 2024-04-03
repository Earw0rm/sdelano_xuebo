#include "vm.h"
#include "pa_alloc.h"
#include "arm/arm_util.h"
#include "printf.h"
#include "base.h"


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

__attribute__((aligned(0x1000))) volatile char kpgtbl[0x1000] = {0};
__attribute__((aligned(0x1000))) volatile char machine_pgtbl[0x1000] = {0};

// Return the address of the PTE in page table pagetable
// that corresponds to virtual address va.  If alloc!=0,
// create any required page-table pages.
pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc, bool unsafe){
    // if va >= maxva then panic 

    for(uint64_t level = 0; level <= 2; ++level){


        uint64_t pgtbl_index = VA_PTBL_IND(va, level);
        pte_t *pte = &pagetable[pgtbl_index];

        if(((*pte) & VALID_DESCRIPTOR) == 1){ // mean that PTE is valid
            pagetable = (pagetable_t)DAADDR((uint64_t)*pte);

        }else{
            if(!alloc) return 0;
            
            uint64_t page;
            
            if(unsafe) page = get_page_unsafe();
            else page = get_page();

            zero_range((uint64_t *) page, PAGE_SIZE);
            
            if((page & 0xfffull) != 0) return 0; // mean that they are not page aligned

            *pte = (page | TABLE_DESCRIPTOR | VALID_DESCRIPTOR);
            pagetable = (pagetable_t)page;
        }

    }


    return &pagetable[VA_PTBL_IND(va, 3)]; // last 3nd level without offset
}


// address round down before map. Support only per page mapping
uint64_t mapva(uint64_t va, uint64_t pa, pagetable_t pgtbl, mair_ind ind, sharability_flag sflag, bool unsafe_alloc){
    va = PGROUNDDOWN(va);
    pa = PGROUNDDOWN(pa);

    if((va & 0xfffull) != 0 || (pa & 0xfffull) != 0) return -1;

    pte_t* pte =  walk(pgtbl, va, true, unsafe_alloc);
    if(pte == 0){
        return -1;
    }
    

    // CHECK PAGE LOWER ATTRIBUTES AND UPPER ATTRIBUTES
    uint64_t page_str = (pa | VALID_DESCRIPTOR | PAGE_DESCRIPTOR | ind | ACCESS_FLAG | ACCESS_PERMISSION | sflag);

    (*pte) = page_str;
    return 0;
}

//identity mapping for code and variables
//0xffff << 48 + addr mapping for devices
uint8_t machine_pgtbl_init(void){
    pagetable_t pgtbl = (pagetable_t) &machine_pgtbl;
    // general all kernel code and variables
    for(char * pointer = 0; pointer < PA_KERNEL_END; pointer += 0x1000){
        uint64_t res = mapva((uint64_t) pointer, 
                                            (uint64_t) pointer,
                                            pgtbl,
                                            NORMAL_NC, 
                                            NON_SHAREABLE, true);
        if(res < 0) return -1;
    }
    // shared kernel 
    for(char * pointer = ((char *) PA_THREAD_SHARED_DATA_BEGIN); pointer < ((char *) PA_THREAD_SHARED_DATA_END); pointer += 0x1000){
        uint64_t res = mapva((uint64_t) pointer,
                                            (uint64_t) pointer, 
                                            pgtbl,
                                            NORMAL_NC,
                                            INNER_SHAREABLE, true);
        if(res < 0) return -2;
    }
    //devices
    for(char * pointer = ((char *) MAIN_PERIPHERAL_BOT); pointer < ((char *) ARM_LOCAL_PERIPHERAL_TOP); pointer += 0x1000){
        uint64_t res = mapva((VAKERN_BASE | ((uint64_t) pointer)),
                                            (uint64_t) pointer, 
                                            pgtbl,
                                            DEVICE,
                                            NON_SHAREABLE, true);
        if(res < 0) return -3;
    }
    return 0;
}

uint8_t kpgtbl_init(void){
    pagetable_t pgtbl = (pagetable_t) &kpgtbl;


    // general all kernel code and variables
    for(char * pointer = 0; pointer < PA_KERNEL_END; pointer += 0x1000){
        uint64_t res = mapva((VAKERN_BASE | ((uint64_t) pointer)), 
                                            (uint64_t) pointer,
                                            pgtbl,
                                            NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT, 
                                            NON_SHAREABLE, true);
        if(res < 0) return -1;
    }

    // shared kernel 
    for(char * pointer = ((char *) PA_THREAD_SHARED_DATA_BEGIN); pointer < ((char *) PA_THREAD_SHARED_DATA_END); pointer += 0x1000){
        uint64_t res = mapva((VAKERN_BASE | ((uint64_t) pointer)),
                                            (uint64_t) pointer, 
                                            pgtbl,
                                            NORMAL_NC,
                                            INNER_SHAREABLE, true);
        if(res < 0) return -2;
    }
    //devices
    for(char * pointer = ((char *) MAIN_PERIPHERAL_BOT); pointer < ((char *) ARM_LOCAL_PERIPHERAL_TOP); pointer += 0x1000){
        uint64_t res = mapva((VAKERN_BASE | ((uint64_t) pointer)),
                                            (uint64_t) pointer, 
                                            pgtbl,
                                            DEVICE,
                                            NON_SHAREABLE, true);
        if(res < 0) return -3;
    }
    return 0;
}

inline static void kpgtbl_debug_print_l(pagetable_t pgtbl, uint8_t level){

    for(uint8_t i = 0; (i < level && level != 3); ++i){
        printf("\t");
    }

    for(pagetable_t pgtbl_p = pgtbl; pgtbl_p < (pgtbl + 512); ++pgtbl_p){
        
        if(level != 3 && (((*pgtbl_p) & VALID_DESCRIPTOR) == 1)){
            uint64_t addr = DAADDR(*pgtbl_p);
            printf("addr:%x \r\n", addr);

            pagetable_t next_pgtbl = (pagetable_t)addr;
            kpgtbl_debug_print_l(next_pgtbl, ++level);
        }else if(level == 3 && ((*pgtbl_p) & VALID_DESCRIPTOR) == 1){
            printf("\t\t\t");
            uint64_t pa = DAADDR(*pgtbl_p);
            printf("pa:%x \r\n", pa);
        }

    }

} 

//call only from el3
void kpgtbl_debug_print(pagetable_t pgtbl){
    printf("################################\r\n");
    kpgtbl_debug_print_l(pgtbl, 0);
    printf("################################\r\n");
}

