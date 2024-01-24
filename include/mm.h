#ifndef MM_H
#define MM_H



#define PAGE_SHIFT  12 // 2^12 = 4096 bytes
#define TABLE_SHIFT 9  // 2^9  =  512 bytes
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define EL3_MASTER_STACK ((2 * SECTION_SIZE) - 4096)
#define EL1_MASTER_STACK (EL3_MASTER_STACK + 4096)



#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned long n);


#endif 

#endif