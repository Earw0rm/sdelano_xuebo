#ifndef _FORK_H
#define _FORK_H

#include "common.h"

int64_t copy_process(uint64_t fn_addr, uint64_t arg);

extern void fork_ret(void); // this is function defined in fork.S

#endif 