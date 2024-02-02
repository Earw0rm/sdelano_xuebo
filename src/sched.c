#include "sched.h"



void preempt_disable(void){
    current->preempt_count--;
}

void preempt_enable(void){
    current->preempt_count++;
}