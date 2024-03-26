#ifndef _VM_H
#define _VM_H

// Table descriptor for 4KB granule size (5853)
// Only Block descriptor and Page descriptor has attributes
// Entry of PGD, PUD, PMD which point to a page table
// ***** Table descriptor 
// +-----+------------------------------+---------+--+
// |     | next level table's phys addr | ignored |11|
// +-----+------------------------------+---------+--+
//      47                             12         2  0
/**
 * [63:59] upper attributes
 * 63:      Non secure table (NSTable) (for stage 1 in non secure res0)
 * [62:61]: Access Permission Talbe 
 * 60:      UXNTable (Unpriveleged Execute Never)
 * 59:      Priveleged Execute Never
*/


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
/**
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
* If a translation stage supports two VA ranges, then all of the following TCR_ELx.TnSZ fields specify the IA size:
* ** TCR_ELx.T0SZ configures the IA size of the lower VA range, translated using TTBR0_ELx
* ** TCR_ELx.T1SZ configures the IA size of the upper VA range, translated using TTBR1_ELx.
*
* The lower VA range is 0x0000000000000000 to (2^(64-T0SZ) - 1)
* The upper VA range is (2^64 - 2^(64-T1SZ)) to 0xFFFFFFFFFFFFFFFF.
*/
#define TCR_T0SZ ((64 - 48) << 0)
#define TCR_T1SZ ((64 - 48) << 16)

// granule size 4kb for TTBR0
#define TCR_TG0_4K (0 << 14)
// granule size 4kb for TTBR1
#define TCR_TG1_4K (2 << 30)

/**
 *  TCR_EL1.DS[59] 
 ***0b0: Bits[49:48] of translation descriptors are RES0.
 *       Bits[9:8] in Block and Page descriptors encode shareability information in the SH[1:0] field
 *       Bits[9:8] in Table descriptors are ignored by hardware.
 *       The minimum value of the TCR_EL1.{T0SZ, T1SZ} fields is 16.
 *       Any memory access using a smaller value generates a stage 1 level 0 translation table fault.
 * 
 ***0b1: Bits[49:48] of translation descriptors hold output address[49:48]. 
 *       Bits[9:8] of Translation table descriptors hold output address[51:50].
 *       The shareability information of Block and Page descriptors for cacheable locations is determined by:
 * 
 *       * TCR_EL1.SH0 if the VA is translated using tables pointed to by TTBR0_EL1
 *       * TCR_EL1.SH1 if the VA is translated using tables pointed to by TTBR1_EL1.
 *       The minimum value of the TCR_EL1.{T0SZ, T1SZ} fields is 12. Any memory access using a smaller value
 *       generates a stage 1 level 0 translation table fault. 
 * 
 *       All calculations of the stage 1 base address are modified for tables of fewer than 8 
 *       entries so that the table is aligned to 64 bytes.  
 * 
 *      Bits[5:2] of TTBR0_EL1 or TTBR1_EL1 are used to hold bits[51:48] of the output 
 *      address in all cases.
 **/

#define TCR_DS (0ull << 59)

/*
* TCR_IPS(Output address size configuration). Sinse TCR_DS = 0, and granularity is 4kb = maximum possible size is 48.
* This field cannot configure OA greater then maximum PA size (ID_AA64MMFR0_EL1.PARange).
* 0b000 4GB 32 bits,   OA[31:0]
* 0b001 64GB 36 bits,  OA[35:0]
* 0b010 1TB 40 bits,   OA[39:0]
* 0b011 4TB 42 bits,   OA[41:0]
* 0b100 16TB 44 bits,  OA[43:0]
* 0b101 256TB 48 bits, OA[47:0]
* 0b110 4PB 52 bits,   OA[51:0]
*/
#define TCR_IPS (0x5ull << 32)

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


//                            Virtual address                                                              
// +-----------------------------------------------------------------------+                                
// |         | PGD Index | PUD Index | PMD Index | PTE Index | Page offset |                              
// +-----------------------------------------------------------------------+ 
// 63     48|-> 47       39|38       30|29       21|20       12|11            0



#define VA_PTBL_IND(va, level) (((va) & ((0x1ffull) << (39 - level*9))) >> (39 - level*9))
#define VA_PTBL_OFFSET(va) (va & 0xfff)



#define PTE2PA(pte) (((*pte) & ((1ull << 48) - 1)) >> 12)
#define PA2PTE(addr) (addr << 12)
#define DAADDR(addr) ((addr & ((1ull << 48) - 1)) & (~(0xfffull)))
#define PGROUNDDOWN(addr) ((addr) & ~(4096 - 1))

#define TABLE_DESCRIPTOR        1 << 1
#define BLOCK_DESCRIPTOR        0 << 1
#define PAGE_DESCRIPTOR         1 << 1
#define VALID_DESCRIPTOR        1



//Software management of the Access flag 5876
//If flag does not set, then generate sync exception 
#define ACCESS_FLAG             0x1 << 10


// lower attributes of page or block descriptor
// The shareable attribute is used to define whether a location is shared with multiple cores.
// Marking a region as Non-shareable means that it is only used by this core, whereas marking it
// as inner shareable or outer shareable, or both, means that the location is shared with other
// observers, for example, a GPU or DMA device might be considered another observer. In the
// same way, the division between inner and outer is IMPLEMENTATION DEFINED.
#define NON_SHAREABLE   (0b00 << 6)
#define OUTER_SHAREABLE (0b10 << 6)
#define INNER_SHAREABLE (0b11 << 6)
/**
 * Summary of instruction access and execution permissions for stage 1 translations 5871
 * 
 * AP[2:1] | Access from higher Exception level | Access from EL0
 * 00      | Read/write                         | None
 * 01      | Read/write                         | Read/write
 * 10      | Read-only                          | None
 * 11      | Read-only                          | Read-only
 * 
*/
#define ACCESS_PERMISSION		(0x00 << 6) 


#ifndef __ASSEMBLER__
#include "common.h"

extern volatile char kpgtbl[];



/**
 *  arm a profile architecture reference manual, page 5835, Table  "finding the descriptor address", contains
 *  full algorithm of resolving descriptor.
 * 
 * 
 * For the 4KB translation granule, when a stage 1 translation table walk is started, the initial lookup level 
 * is determined by the value of the TCR_ELx.TnSZ field as shown in the following table.
 * 
 * 1) Initial lookup level 
 * 2) TnSZ minimum value 
 * 3) Maximum IA bits resolved
 * 4) TnSZ maximum value
 * 5) Minimum IA bits resolved
 * 6) Additional requirements
 * 
 *  1  | 2  | 3         | 4  | 5         | 6                                  |
 * -1  | 12 | IA[52:12] | 15 | IA[48:12] | Effective value of TCR_ELx.DS is 1 |
 *  0  | 17 | IA[47:12] | 24 | IA[39:12] | -                                  |
 *  1  | 25 | IA[38:12] | 33 | IA[38:12] | -                                  |
 *  2  | 34 | IA[29:12] | 39 | IA[24:12] | -                                  |
 *  3  | 40 | IA[23:12] | 42 | IA[21:12] | FEAT_TTST is implemented           | 
 *  4  | 43 | IA[20:12] | 48 | IA[15:12] | FEAT_TTST is implemented           |  
 *   
*/
pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc);
uint64_t mapva(uint64_t va, uint64_t pa, pagetable_t pgtbl, mair_ind ind);
pagetable_t init_mmu(init_mmu);
#endif

#endif  