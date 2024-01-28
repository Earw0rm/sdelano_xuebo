#ifndef MM_H
#define MM_H



#define PAGE_SHIFT  12 // 2^12 = 4096 bytes
#define TABLE_SHIFT 9  // 2^9  =  512 bytes
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define SP_EL0_MASTER_STACK ((2 * SECTION_SIZE) - 8192)
#define SP_EL1_MASTER_STACK (SP_EL0_MASTER_STACK + 4096)
#define SP_EL3_MASTER_STACK (SP_EL0_MASTER_STACK + 8192)



#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned long n);


#endif 

#endif