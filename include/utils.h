#ifndef _UTILS_H
#define _UTILS_H
#include "common.h"
// todo legasy boolsheet

extern void put32(uint64_t addr, uint32_t val);

extern uint32_t get32(uint64_t addr);

extern uint32_t get_el(void);

void delay(uint32_t tick);

#endif