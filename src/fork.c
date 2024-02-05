#include "fork.h"
#include "pa_alloc.h"
#include "sched.h"

#include "printf.h"
#include "mini_uart.h"

// An important thing to understand about the copy_process function is that after it finishes execution,
// no context switch happens. The function only prepares new task_struct and adds it to the task array —
// this task will be executed only after schedule function is called.
int64_t copy_process(uint64_t fn_addr, uint64_t arg){
    init_printf(0, putc);

    // Preemption is disabled because we don't want to be rescheduled 
    // to a different task in the middle of the copy_process function
    preempt_disable(); 

    // IMPORTANT!!!!
    // task_struct will be at the bottom of the PAGE. This mean that address of state will be bigger than
    // address cpu_context for example.
    struct task_struct * p = (struct task_struct *) get_page(); 
    if(p == 0) return 1;
    p->priority = current->priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt_count = 1; // disable preemtion until shedule_tail



    p->cpu_context.x19 = fn_addr;
    p->cpu_context.x20 = arg;
    p->cpu_context.x31_sp = ((uint64_t) TOP_OF_THE_PAGE(p)); // todo Crutch
    p->cpu_context.x30_lr = ((uint64_t) &fork_ret);
        
    printf("Copy process pc %u sp %u \r \n", p->cpu_context.x30_lr, p->cpu_context.x31_sp);
    printf("Page addr %x, TOP_OF_THE_PAGE %x \r \n", p, TOP_OF_THE_PAGE(p));
    int pid = number_of_runnning_tasks++;
    task[pid] = p;
    preempt_enable();
    return 0;
}

