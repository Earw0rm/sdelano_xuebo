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
// (47 - ((level + 1)*9))
#ifndef __ASSEMBLER__
#include "common.h"


pte_t * walk(pagetable_t pagetable, uint64_t va, bool alloc);

#endif

#endif  