#ifndef _VM_H
#define _VM_H



#define VAKERN_BASE ((uint64_t) 0xfffffff << 35)


// part of TCR_EL1
// 2^(64 - T*SZ) addresses 
#define TCR_T0SZ (64 - 35)
#define TCR_T1SZ ((64 - 35) << 16)

// granule size 4kb for TTBR0
#define TCR_TG0_4K (0 << 14)
// granule size 4kb for TTBR1
#define TCR_TG1_4K (2 << 30)

#define TCR_DS (0 << 59)

#define TCR_IPS (0b001 << 32    )

#define TCR_VALUE (TCR_T0SZ | TCR_T1SZ | TCR_TG0_4K | TCR_TG1_4K | TCR_DS | TCR_IPS)

/* part of MAIR_EL1
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *			n	MAIR
 *   DEVICE_nGnRnE	            000	00000000
 *   NORMAL_NON_CACHABLE		001	01000100
*/

typedef enum {
    DEVICE, 
    NORMAL_NC
} mair_ind;

#define MT_DEVICE_nGnRnE                    (0x00)
#define MT_NORMAL_NON_CACHABLE              (0x1)

#define MT_DEVICE_nGnRnE_FLAGS		        (0x00)
#define MT_NORMAL_NON_CACHABLE_FLAGS  		(0x44)

#define MAIR_VALUE  (MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))





// #define VA_PTBL_IND(va, level) (((va) & (0x1ff << (47 - level*9))) >> (47 - level*9))
#define VA_PTBL_IND(va, level) (((va) & ((0x1ff) << (30 - ((level - 1)*9)))) >> ((0x1ff) << (30 - ((level - 1)*9))))
#define VA_PTBL_OFFSET(va) (va & 0x7ff)

#define PTE2PA(pte) (*pte & (((1ull << 48) - 1) & ((1ull << 12) - 1)))
#define PA2PTE(addr) (addr << 12)

#define TABLE_DESCRIPTOR 1 << 1
#define BLOCK_DESCRIPTOR 0 << 1
#define PAGE_DESCRIPTOR  1 << 1
#define VALID_DESCRIPTOR 1



#ifndef __ASSEMBLER__
#include "common.h"

extern char kpgtbl[];

pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc);
uint64_t mapva(uint64_t va, uint64_t pa, pagetable_t pgtbl, mair_ind ind);
pagetable_t init_mmu(void);
#endif

#endif  