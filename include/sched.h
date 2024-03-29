#ifndef _SCHED_H
#define _SCHED_H



#define CPU_CONTEXT_OFFSET 0
#define TRAPFRAME_OFFSET 104

#ifndef __ASSEMBLER__
#include "common.h"




#include "common.h"
#define THREAD_SIZE 4096
#define NR_TASKS    64
#define FST_TASK    task[0]
#define LST_TASK    task[NR_TASKS-1]


extern struct task_struct *current;
// array for simplisity. Later change to freelist (or hashtable)
extern struct task_struct *task[NR_TASKS];
extern uint64_t number_of_runnning_tasks;


typedef enum {
    TASK_RUNNING
} task_state;

struct cpu_context {
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29_fp; // frame pointer
    uint64_t x30_lr; // link register
    uint64_t x31_sp; // stack pointer for current thread    
};
struct task_struct{
    struct cpu_context cpu_context;
    
    uint64_t stack0;
    uint64_t stack1;

    task_state state;
    // This field is used to determine how long the current task has been running.
    // counter decreases by 1 each timer tick and when it reaches 0 another task is scheduled.
    int64_t counter;
    // When a new task is scheduled its priority is copied to counter. 
    // By setting tasks priority, we can regulate the amount of processor 
    // time that the task gets relative to other tasks.
    int64_t priority;
    // If this field has a non-zero value it is an indicator that right now the current 
    // task is executing some critical function that must not be interrupted 
    // (for example, it runs the scheduling function.). 
    // If timer tick occurs at such time it is ignored and rescheduling is not triggered.
    int64_t preempt_count;



    uint64_t flags;
};

// After the kernel startup, there is only one task running:
// the one that runs kernel_main function. It is called "init task".
// Before the scheduler functionality is enabled, we must fill task_struct corresponding
// to the init task. This is done here.
#define INIT_TASK { \
    .cpu_context   = {0}, \
    .stack0        = 0,   \
    .stack1        = 0,   \
    .state         = 0,   \
    .counter       = 0,   \
    .priority      = 1,   \
    .preempt_count = 1,   \
    .flags         = 1    \
}

// void init_task_initialization(uint64_t stack0, uint64_t stack1);

void preempt_disable(void);
void preempt_enable(void);
void schedule(void);
void timer_tick(void);
void switch_to(struct task_struct * next);
extern void cpu_switch_to(struct task_struct * prev, struct task_struct * next);
extern bool init_task_is_initialized;

#endif
#endif 