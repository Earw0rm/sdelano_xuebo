#include "sched.h"
#include "arm/arm_util.h"


#include "mini_uart.h"
#include "printf.h"


static struct task_struct init_task = INIT_TASK;
struct task_struct * current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
uint64_t number_of_runnning_tasks = 1;

bool init_task_is_initialized = false;

void init_task_initialization(uint64_t stack0, uint64_t stack1){
    init_task.stack0 = stack0;
    init_task.stack1 = stack1;
    
    init_task_is_initialized = true;
}

void preempt_disable(void){
    current->preempt_count++;
}

void preempt_enable(void){
    if(current->preempt_count > 0){
        current->preempt_count -= 1; //overflow safe
    }
}

void switch_to(struct task_struct * next){
    if(current == next) return;
    struct task_struct * prev = current;
    current = next; // TODO amoswap?
    // printf("switch to %x %x \r\n", prev, next);
    // printf("lr = %x, sp = %x \r\n", next->cpu_context.x30_lr, next->cpu_context.x31_sp);

    cpu_switch_to(prev, next);
}


// based on https://github.com/zavg/linux-0.01/blob/master/kernel/sched.c#L68
// It's amazing, right?
void _schedule(void){
    // this ensures that nested schedule will not be called
    preempt_disable(); 
    init_printf(0, putc);
    uint64_t next = 0;
    int64_t c = -1;
    struct task_struct * p;

    while(true){
        // trying to find the maximum value of p->counter
        for(uint64_t i = 0; i < NR_TASKS; ++i){
            p = task[i];
            if(p && (p->state == TASK_RUNNING) && (p->counter > c)){
                c = p->counter;
                next = i;
            }
        } 
        // If no such task is found this means that no tasks in TASK_RUNNING state currently exist 
        // or all such tasks have 0 counters
        if(c) break;
        // In a real OS, the first case might happen, for example, when all tasks are waiting for an interrupt. 
        // **** This is some form of deadlock? If so, then second for exists for prevent situations like that.
        // In this case, the second nested for loop is executed.
        // For each task (no matter what state it is in) this loop increases its counter. 
        // The counter increase is done in a very smart way, such that a task counter can never get larger than 2 * priority
        for(uint64_t i = 0; i < NR_TASKS; ++i){
            p = task[i];
            if(p){
                p->counter = (p->counter >> 1) + p->priority;
            }
        }
        // only 2 while iterations is possuble when there is exists task in running state.
    }
    switch_to(task[next]);
    preempt_enable();
}

void schedule(void){
    current->counter = 0;
    _schedule();
}

//this function call from timer interrupt handler
void timer_tick(void){
    if(current->counter != 0){  // overflow safe
        current->counter -= 1;
    }
    if(current->counter > 0 || current->preempt_count > 0) return;
    current->counter = 0;
    enable_irq();
    _schedule();
    disable_irq();
}

extern void fork_ret(void);

