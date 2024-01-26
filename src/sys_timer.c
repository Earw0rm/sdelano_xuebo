#include "sys_timer.h"



uint32_t sys_timer_init(void){
    uint32_t cl = SYSTEM_TIMER->counter_lower;
    SYSTEM_TIMER->compare[3] = cl + 12000000;
    SYSTEM_TIMER->control_status = 0b1000U;
    return cl + 12000000;
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