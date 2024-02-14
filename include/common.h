#ifndef COMMON_H
#define COMMON_H

#ifndef __ASSEMBLER__
#include <stdint.h>



typedef volatile uint32_t reg32;
typedef volatile uint64_t reg64;
typedef volatile uint8_t  reg8;

typedef enum {
    false,
    true
} bool;

typedef uint64_t pte_t;
typedef uint64_t* pagetable_t;

#endif // __ASSEMBLER__
#endif