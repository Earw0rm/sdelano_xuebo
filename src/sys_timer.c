#include "sys_timer.h"



uint32_t sys_timer_init(void){
    sys_timer_recharge(3, SCHEDULER_RECHARGE_TIME);
    uint32_t cl = SYSTEM_TIMER->counter_lower;
    return cl + SCHEDULER_RECHARGE_TIME;
}

void sys_timer_recharge(uint32_t timer_num, uint32_t recharge_time){
    if(timer_num > 3) return; //do nothing
    uint32_t cl = SYSTEM_TIMER->counter_lower;
    SYSTEM_TIMER->compare[timer_num] = cl + recharge_time;
    SYSTEM_TIMER->control_status = (1 << timer_num);
}
