#ifndef _VM_H
#define _VM_H

#define KERNEL_VA_BASE 0xffff000000000000
#define USER_VA_BASE 0x0000000000000000


#define MAX_KERNEL_VA (KERNEL_VA_BASE + 0xffff_ffff_ffff)   
#define MAX_USER_VA (USER_VA_BASE + 0xffff_ffff_ffff)

#define VA_PTBL_IND(va, level) (((va) & (0x1ff << (47 - (level + 1)*9))) >> (47 - (level + 1)*9))
#define VA_PTBL_OFFSET(va) (va & 0x7ff)

#define PTE2PA(pte) (*pte & (((1ull << 48) - 1) & ((1ull << 12) - 1)))
#define PA2PTE(addr) (addr << 12)

#define BLOCK_DESCRIPTOR 1 << 1
#define VALID_DESCRIPTOR 1



// part of TCR_EL1
// 2^(64 - T*SZ) addresses 
#define TCR_T0SZ (64 - 48)
#define TCR_T1SZ ((64 - 48) << 16)

// granule size 4kb for TTBR0
#define TCR_TG0_4K (0 << 14)
// granule size 4kb for TTBR1
#define TCR_TG1_4K (2 << 30)

#define TCR_VALUE (TCR_T0SZ | TCR_T1SZ | TCR_TG0_4K | TCR_TG1_4K)

/* part of MAIR_EL1
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *			n	MAIR
 *   DEVICE_nGnRnE	            000	00000000
 *   NORMAL_NON_CACHABLE		001	01000100
*/
#define MT_DEVICE_nGnRnE                    (0x00)
#define MT_NORMAL_NON_CACHABLE              (0x1)

#define MT_DEVICE_nGnRnE_FLAGS		        (0x00)
#define MT_NORMAL_NON_CACHABLE_FLAGS  		(0x44)

#define MAIR_VALUE  (MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))

/*
* #define MAIR_VALUE (MT_DEVICE_nGnRnE_FLAGS << (8 x | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))
*/

#ifndef __ASSEMBLER__
#include "common.h"


pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc);

void init_mmu(void);

#endif

#endif  