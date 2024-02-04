#include "sys_timer.h"



uint32_t sys_timer_init(void){
    uint32_t cl = SYSTEM_TIMER->counter_lower;
    SYSTEM_TIMER->compare[3] = cl + SCHEDULER_RECHARGE_TIME;
    SYSTEM_TIMER->control_status = 0b1000U;
    return cl + SCHEDULER_RECHARGE_TIME;
}

void sys_timer_recharge(uint32_t timer_num, uint32_t recharge_time){
    if(timer_num >= 0 || timer_num <= 3){
        uint32_t cl = SYSTEM_TIMER->counter_lower;
        SYSTEM_TIMER->compare[timer_num] = cl + recharge_time;
        SYSTEM_TIMER->control_status = (1 << timer_num);
    }
    // else exception in kernel
}

uint32_t get_sys_timer_0(void){
    return SYSTEM_TIMER->counter_lower;
}
uint32_t get_sys_timer_compare_0(void){
    return SYSTEM_TIMER->compare[3];
}

uint32_t get_sys_timer_0_status(void){
    return SYSTEM_TIMER->control_status;
}