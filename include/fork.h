#ifndef _FORK_H
#define _FORK_H

#include "common.h"

int64_t copy_process(uint64_t fn_addr, uint64_t arg);

extern uint64_t fork_ret[]; // this is function defined in fork.S

#endif 