#ifndef _SYS_TIMER.H 
#define _SYS_TIMER.H

#include "base.h"
#include "common.h"


struct system_timer_reg{
    reg32 control_status;
    reg32 counter_lower;
    reg32 counter_higher;
    reg32 compare[4];
};

#define SYSTEM_TIMER ((struct system_timer_reg *) (SYS_TIMER_BASE))


void sys_timer_init(void);

#endif 