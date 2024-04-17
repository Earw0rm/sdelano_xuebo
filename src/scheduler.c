#include "scheduler.h"
#include "arm/arm_util.h"
#include "speenlock.h"
#include "vm.h"
#include "pa_alloc.h"
#include "memlayout.h"
#include "GIC400.h"

__attribute__((section(".data.thread_shared")))
static struct speenlock tasks_lock = {
    .cpu_num = -1,
    .locked = 0,
    .name = "clear"
};

extern void urestore_and_ret(void); // exception.S
extern void _switch_to(struct kcontext * from, struct kcontext * to);
//current task live hire 
struct cpu cpus[4]           = {0};
//acquire lock for changes this 
static int64_t tasks_count   = -1 ;
static struct task tasks[64] = {0};

//completed tasks
static int64_t tasks_buffer_count   = -1 ;
static struct task task_buffer[64]  = {0};


void release_restore_return(void){
    release(&tasks_lock);
    gic400_eoi(SYS_TIMER_3);
    urestore_and_ret();
}

// return {0} if tasks[] has 0 task
struct task get_last(void){
    struct task ret = {0};
    acquire(&tasks_lock);

        if(tasks_count >= 0){
            ret = tasks[tasks_count];
            --tasks_count;
        }

    release(&tasks_lock);
    return ret;
}

// this can be called only when pagetable is exists and they are ready
struct cpu * my_cpu(void){
    return (struct cpu *) LAYOUT_MY_CPU(get_processor_id());
}

void schedule(void){
    struct task task = get_last();
    if(!task.pure){
        acquire(&tasks_lock);   
            if(tasks_buffer_count >= 0){
                    for(;tasks_buffer_count >= 0; --tasks_buffer_count){
                    tasks[++tasks_count] = task_buffer[tasks_buffer_count]; 
                }
            }
        release(&tasks_lock);   
        return;
    }
    switch_to(&(task)); 
}

//need to be return throo kernel_ret
void switch_to(struct task * new_task){ 
    acquire(&tasks_lock);
        my_cpu()->current_task = *new_task;
        _switch_to(&(task_buffer[++tasks_buffer_count].kctx), &(new_task->kctx));
    release(&tasks_lock);
}

uint8_t fork(uint8_t (*main)(void)){
    struct task task = create_task(main, EL0t_INTR_ON);
    acquire(&tasks_lock);
        uint8_t task_id = ++tasks_buffer_count;
        task_buffer[task_id] = task;
    release(&tasks_lock);
    return task_id;
}


//todo как обрабатывать user pagetable
//какая она должна быть? 
struct task create_task(uint8_t (*main)(void), uint64_t spsr_el1){
    struct task task = {0};
    
    uint64_t sp_el0_page = get_page();
    uint64_t sp_el1_page = get_page();
    uint64_t ttbr0_page = get_page();
    uint64_t ttbr1_page = get_page();

    zero_range((char *)sp_el0_page, 0x1000);
    zero_range((char *)sp_el1_page, 0x1000);
    
    zero_range((char *)ttbr0_page,  0x1000);
    zero_range((char *)ttbr1_page,  0x1000);            

    uint8_t init_res = create_kernel_pagetable((pagetable_t) ttbr1_page, false);
    // we need to handle upgtbl_init

    task.uctx.sp_el0 = sp_el0_page;
    task.kctx.sp_el1 = sp_el1_page;
    task.ttbr0_el1 = ttbr0_page;    


    task.uctx.elr_el1  = (uint64_t) main;
    task.uctx.spsr_el1 = spsr_el1;


    task.pure = true;
    
     // sinse this is first time, we just restore all user regs and jump inside main in EL1 regime
    task.kctx.x30_lr = &release_restore_return;


    return task;
}