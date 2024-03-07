#ifndef _VM_H
#define _VM_H


// Only Block descriptor and Page descriptor has attributes
// Entry of PGD, PUD, PMD which point to a page table
// ***** Table descriptor f
// +-----+------------------------------+---------+--+
// |     | next level table's phys addr | ignored |11|
// +-----+------------------------------+---------+--+
//      47                             12         2  0

// Entry of PUD, PMD which point to a block
// ***** Block descriptor
// +-----+------------------------------+---------+--+
// |     |  block's physical address    |attribute|01|
// +-----+------------------------------+---------+--+
//      47                              n         2  0

// Entry of PTE which point to a page
// ***** Page descriptor
// +-----+------------------------------+---------+--+
// |     |  page's physical address     |attribute|11|
// +-----+------------------------------+---------+--+
//      47                             12         2  0

// Invalid entry
// +-----+------------------------------+---------+--+
// |     |  page's physical address     |attribute|*0|
// +-----+------------------------------+---------+--+
//      47                             12         2  0
/*

* [63:50] => page and block upper attrubutes
* Bits[54] The unprivileged execute-never bit, non-executable page frame for EL0 if set.
* Bits[53] The privileged execute-never bit, non-executable page frame for EL1 if set.
* Bits[47:n] The physical address the entry point to. Note that the address should be aligned to 2n Byte.
* Bits[10] The access flag, a page fault is generated if not set.
* Bits[7] 0 for read-write, 1 for read-only.
* Bits[6] 0 for only kernel access, 1 for user/kernel access.
* Bits[4:2] The index to MAIR.
* Bits[1:0] Specify the next level is a block/page, page table, or invalid.

* Basic mmu configuration 
* 1)Disable instruction cache.
* 2)Disable data cache.
* 3)The addressable region is 48 bit.
* 4)The page granule size is 4KB.
* 5)Not use address space ID (ASID).
*/
#define VAKERN_BASE ((uint64_t) 0xffff << 48)


// part of TCR_EL1
// 2^(64 - T*SZ) addresses 
/*
* Determining the maximum permitted TnSZ value
* Translation granule size |  FEAT_TTST not implemented | FEAT_TTST implemented
* 4KB                           39                          48
* 16KB                          39                          48
* 64KB                          39                          47
*
* If TCR_ELx.TnSZ configures an IA size that is smaller than the maximum size, then each one-bit reduction in the
* IA size has one of the following effects on the lookup level that the table walk starts with:
* 1) The translation table size is reduced by one half.
* 2) The table walk starts one level later.
*
*
*/
#define TCR_T0SZ ((64 - 48) << 0)
#define TCR_T1SZ ((64 - 48) << 16)

// granule size 4kb for TTBR0
#define TCR_TG0_4K (0b00 << 14)
// granule size 4kb for TTBR1
#define TCR_TG1_4K (0b10 << 30)

#define TCR_DS (0 << 59)

/*
* TCR_IPS:
* 0b000 4GB 32 bits,   OA[31:0]
* 0b001 64GB 36 bits,  OA[35:0]
* 0b010 1TB 40 bits,   OA[39:0]
* 0b011 4TB 42 bits,   OA[41:0]
* 0b100 16TB 44 bits,  OA[43:0]
* 0b101 256TB 48 bits, OA[47:0]
* 0b110 4PB 52 bits,   OA[51:0]
*/
#define TCR_IPS (0b101 << 32)

#define TCR_VALUE (TCR_T0SZ | TCR_T1SZ | TCR_TG0_4K | TCR_TG1_4K )

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

#define MAIR_VALUE  (MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NON_CACHABLE_FLAGS << (8 * MT_NORMAL_NON_CACHABLE))



//                            Virtual address                                                              
// +-----------------------------------------------------------------------+                                
// |         | PGD Index | PUD Index | PMD Index | PTE Index | Page offset |                              
// +-----------------------------------------------------------------------+ 
// 63     48|47       39|38       30|29       21|20       12|11            0



#define VA_PTBL_IND(va, level) (((va) & ((0x1ffull) << (39 - level*9))) >> (39 - level*9))
#define VA_PTBL_OFFSET(va) (va & 0xfff)



#define PTE2PA(pte) (((*pte) & ((1ull << 48) - 1)) >> 12)
#define PA2PTE(addr) (addr << 12)
#define PGROUNDDOWN(addr) ((addr) & ~(4096 - 1))
#define TABLE_DESCRIPTOR 1 << 1
#define BLOCK_DESCRIPTOR 0 << 1
#define PAGE_DESCRIPTOR  1 << 1
#define VALID_DESCRIPTOR 1



#ifndef __ASSEMBLER__
#include "common.h"

extern volatile char kpgtbl[];

pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc);
uint64_t mapva(uint64_t va, uint64_t pa, pagetable_t pgtbl, mair_ind ind);
pagetable_t init_mmu(void);
#endif

#endif  