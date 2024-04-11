#include "scheduler.h"
#include "arm/arm_util.h"
#include "speenlock.h"
#include "vm.h"
#include "pa_alloc.h"
#include "memlayout.h"


__attribute__((section(".data.thread_shared")))
static struct speenlock tasks_lock = {
    .cpu_num = -1,
    .locked = 0,
    .name = "clear"
};

extern void urestore_and_ret(void); // exception.S

//current task live hire 
struct cpu cpus[4]           = {0};
//acquire lock for changes this 
static uint64_t tasks_count  =  0 ;
static struct task tasks[64] = {0};

//completed tasks
static uint64_t tasks_buffer_count  =  0 ;
static struct task task_buffer[64]  = {0};


// return 0 if tasks[] has 0 task
struct task get_last(void){
    struct task ret = {0};
    acquire(&tasks_lock);
    if(tasks_count == 0){
        release(&tasks_lock);
        return ret;
    }

    ret = tasks[tasks_count];
    --tasks_count;
    release(&tasks_lock);
    return ret;
}

// this can be called only when pagetable is exists and they are ready
struct cpu * my_cpu(void){
    return (struct cpu *) LAYOUT_MY_CPU;
}

void schedule(void){
    struct task task = get_last();
    if(!task.pure) return;// try to fill buffer and ret
    switch_to(task); // never return from this
}

//need to be return throo kernel_ret
void switch_to(struct task task){ 
    struct cpu * mycpu = my_cpu();
    struct task old_task = mycpu->current_task;
    acquire(&tasks_lock);
    task_buffer[tasks_buffer_count] = old_task;
    mycpu->current_task = task;
    release(&tasks_lock);

    //намазать
}

struct task create_task(uint8_t (*main)(void)){
    struct task task = {0};
    
    uint64_t sp_el0_page = get_page();
    uint64_t sp_el1_page = get_page();

    uint64_t ttbr0_page = get_page();
    uint64_t ttbr1_page = get_page();

    zero_range((char *)sp_el0_page, 0x1000);
    zero_range((char *)sp_el1_page, 0x1000);
    zero_range((char *)ttbr0_page,  0x1000);
    zero_range((char *)ttbr1_page,  0x1000);            

    uint8_t init_res = kpgtbl_init((pagetable_t) ttbr1_page);
    // we need to handle upgtbl_init

    task.uctx.sp_el0 = sp_el0_page;
    task.kctx.sp_el1 = sp_el1_page;
    task.ttbr0_el1 = ttbr0_page;    
    task.ttbr1_el1 = ttbr1_page;

    task.uctx.elr_el1  = (uint64_t) main;
    task.uctx.spsr_el1 = EL0_INTR_ON;


    task.pure = true;
    
     // sinse this is first time, we just restore all user regs and jump inside main in EL1 regime
    task.kctx.x30_lr = &urestore_and_ret;


    return task;
}