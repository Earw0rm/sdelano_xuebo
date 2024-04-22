#include "vm.h"
#include "pa_alloc.h"
#include "arm/arm_util.h"
#include "printf.h"
#include "base.h"
#include "scheduler.h"
#include "memlayout.h"


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



// Return the address of the PTE in page table pagetable
// that corresponds to virtual address va.  If alloc!=0,
// create any required page-table pages.

__attribute__((aligned(0x1000))) //start from hire
volatile char kpgtbl[0x1000] = {0};

//why its here?
__attribute__((aligned(16)))
volatile char kstack1[0x4000] = {0};

pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc, bool unsafe){
    // if va >= maxva then panic 

    for(uint64_t level = 0; level <= 2; ++level){


        uint64_t pgtbl_index = VA_PTBL_IND(va, level);
        pte_t * pte = &pagetable[pgtbl_index];

        if(((*pte) & VALID_DESCRIPTOR) == 1){ // mean that PTE is valid
            pagetable = (pagetable_t)DAADDR((uint64_t)*pte);
        }else{
            if(!alloc) return (pte_t *) 0x0;
            
            uint64_t page;
            
            if(unsafe) page = get_page_unsafe();
            else page = get_page();

            if(page == 0 || ((page & 0xfffull) != 0)) return (pte_t *) 0x1; // mean that they are not page aligned or something

            zero_range((char *) page, PAGESIZE);
            

            *pte = (page | TABLE_DESCRIPTOR | VALID_DESCRIPTOR);
            pagetable = (pagetable_t)page;
        }

    }


    return &pagetable[VA_PTBL_IND(va, 3)]; // last 3nd level without offset
}


// address round down before map. Support only per page mapping
int8_t mapva(uint64_t va, uint64_t pa, pagetable_t pgtbl,
             mair_ind ind, sharability_flag sflag, 
             uint64_t flags, 
             bool unsafe_alloc){
    va = PGROUNDDOWN(va);
    pa = PGROUNDDOWN(pa);

    if((va & 0xfffull) != 0 || (pa & 0xfffull) != 0) return -3;

    pte_t* pte =  walk(pgtbl, va, true, unsafe_alloc);
    if(pte == (pte_t *) 0x0 || pte == (pte_t *) 0x1){
        return -1;
    }
    

    // CHECK PAGE LOWER ATTRIBUTES AND UPPER ATTRIBUTES
    uint64_t page_str = (pa | flags | ind | ACCESS_FLAG | ACCESS_PERMISSION | sflag);

    (*pte) = page_str;
    return 0;
}



int8_t kpgtbl_init(void){
    //1st memory range
    for(char * pointer = (char *) MEM_TMP_START; pointer < ((char *) MEM_TMP_END); pointer += 0x1000){
        int8_t res = mapva((uint64_t) pointer, (uint64_t) pointer,
                                            (pagetable_t)&kpgtbl,
                                            NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT, 
                                            NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, true);
        if(res < 0) return -1;
    }

    //temp for faster initialization
    // //2nd memory range
    // for(char * pointer = (char *) MEM_VC_BASE_TOP; pointer < ((char *) MEM_SDRAM_TOP); pointer += 0x1000){
    //     int8_t res = mapva((uint64_t) pointer, (uint64_t) pointer,
    //                                         kpgtbl,
    //                                         NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT, 
    //                                         NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, true);
    //     if(res < 0) return -2;
    // }
    // //3rd memory range
    // for(char * pointer = (char *) MEM_SDRAM_BOT; pointer < ((char *) MEM_PASTOP); pointer += 0x1000){
    //     int8_t res = mapva((uint64_t) pointer, (uint64_t) pointer,
    //                                         kpgtbl,
    //                                         NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT, 
    //                                         NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, true);
    //     if(res < 0) return -3;
    // }


    // kernel
    for(char * pointer = ((char *) MEM_KERN_START); pointer < ((char *) MEM_KERN_END); pointer += 0x1000){
        int64_t res = mapva((uint64_t) pointer, (uint64_t) pointer, 
                                            (pagetable_t)&kpgtbl,
                                            NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                            NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, true);
        if(res < 0) return -4;
    }

    char * ps = ((char *) MEM_KERNEL_SHARED_DATA_BEGIN);
    char * pe = ((char *) MEM_KERNEL_SHARED_DATA_END);
    //kernel shared data special memory type
    for(char * pointer = ps; pointer < pe; pointer += 0x1000){
        int64_t res = mapva((uint64_t) pointer, (uint64_t) pointer, 
                                            (pagetable_t)&kpgtbl,
                                            NORMAL_IO_WRITE_BACK_RW_ALLOCATION_NON_TRAINSIENT,
                                            INNER_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, true);
        if(res < 0) return -4;
    }

    //devices
    for(char * pointer = ((char *) MAIN_PERIPHERAL_BOT); pointer < ((char *) ARM_LOCAL_PERIPHERAL_TOP); pointer += 0x1000){
        int64_t res = mapva((uint64_t) pointer, (uint64_t) pointer, 
                                            (pagetable_t)&kpgtbl,
                                            DEVICE,
                                            NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, true);
        if(res < 0) return -5;
    }

    //kstack1
    for(uint8_t i = 0; i <= 3; ++i){
        int64_t res = mapva(KSTACK(i), (uint64_t) &kstack1[i << 12], (pagetable_t)&kpgtbl,
                                                NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, true);
        if(res < 0) return -6;        
    }
    //my_cpus
    for(uint8_t i = 0; i <= 3; ++i){
        int64_t res = mapva(LAYOUT_MY_CPU(i), (uint64_t) &cpus[i], (pagetable_t)&kpgtbl,
                                                NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, true);
        if(res < 0) return -7;        
    }
    return 0;
}   



// kalling only inside kernel. 
// Pagetables can contain physical adress link on next pgtbl
// SO, if address starts from 0x0000, then we change address to start with 0xffff
inline static void kpgtbl_debug_print_l(pagetable_t pgtbl, uint8_t level){
    

    for(uint8_t i = 0; (i < level && level < 3); ++i){
        uprintf("\t");
    }

    for(pagetable_t pgtbl_p = pgtbl; pgtbl_p < (pgtbl + 512); ++pgtbl_p){

        if(level != 3 && (((*pgtbl_p) & VALID_DESCRIPTOR) == 1)){
            uint64_t addr = DAADDR(*pgtbl_p);
            uprintf("addr:%x \r\n", addr);

            pagetable_t next_pgtbl = (pagetable_t)addr;
            kpgtbl_debug_print_l(next_pgtbl, ++level);
        }else if(level == 3 && ((*pgtbl_p) & VALID_DESCRIPTOR) == 1){
            uprintf("\t\t\t");
            uint64_t pa = DAADDR(*pgtbl_p);
            uprintf("pa:%x \t full_pte:%x \r\n", pa, (*pgtbl_p));
        }

    }

} 

//call only from el3
void kpgtbl_debug_print(pagetable_t pgtbl){
    uprintf("################################\r\n");
    kpgtbl_debug_print_l(pgtbl, 0);
    uprintf("################################\r\n");
}

