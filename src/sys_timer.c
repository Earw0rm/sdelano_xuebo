#include "sys_timer.h"



void sys_timer_init(void){
    uint32_t cl = SYSTEM_TIMER->counter_lower;
    cl += 3000000;
    SYSTEM_TIMER->compare[0] = cl;
}