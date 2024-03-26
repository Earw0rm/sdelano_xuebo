#ifndef _SYS_TIMER_H 
#define _SYS_TIMER_H

#include "base.h"
#include "common.h"


struct system_timer_reg{
    reg32 control_status;
    reg32 counter_lower;
    reg32 counter_higher;
    // timer 0 and 2 reserve for GPU. 1 and 3 for CPU usages. 
    reg32 compare[4];
};

#define SYSTEM_TIMER ((struct system_timer_reg *) (SYS_TIMER_BASE))
#define SCHEDULER_RECHARGE_TIME 120000

uint32_t sys_timer_init(void);
void sys_timer_recharge(uint32_t timer_num, uint32_t recharge_time);


#endif 