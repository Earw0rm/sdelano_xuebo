#ifndef _UTILS_H
#define _UTILS_H
#include "common.h"

#define CALC_RANGE(range, type) ((range) / sizeof(type))

extern void put32(uint64_t addr, uint32_t val);

extern uint32_t get32(uint64_t addr);

extern uint32_t get_el(void);



#endif