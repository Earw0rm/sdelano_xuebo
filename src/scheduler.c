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



//current task live hire 
struct cpu cpus[4]           = {0};
//acquire lock for changes this 
static int64_t tasks_count   = -1 ;
static struct task tasks[64] = {0};

//completed tasks
static int64_t tasks_buffer_count   = -1 ;
static struct task task_buffer[64]  = {0};



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
    struct task old_task = my_cpu()->current_task;
    my_cpu()->current_task = old_task;

    acquire(&tasks_lock);
        task_buffer[++tasks_buffer_count] = *new_task;
    release(&tasks_lock);
}

uint8_t fork(uint8_t (*main)(void)){
    struct task task = user_task_create(main);
    acquire(&tasks_lock);
        uint8_t task_id = ++tasks_buffer_count;
        task_buffer[task_id] = task;
    release(&tasks_lock);
    return task_id;
}


struct task user_task_create(uint8_t (*main)(void)){
            struct task task = {0};

            pagetable_t pgtbl = (pagetable_t) get_page();
            zero_range((char *)pgtbl, 0x1000);

            uint64_t tf_page = get_page();
            zero_range((char *)tf_page, 0x1000);


            int64_t res = mapva(MEM_USER_TRAPFRAME, tf_page, pgtbl,
                                                NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);
            
            uint64_t stack_page = get_page();
            zero_range((char *)stack_page, 0x1000);

            int64_t res = mapva(MEM_USER_STACK, stack_page, pgtbl,
                                                NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);                                                             
            

            for(char * pointer = (char *) MEM_USER_TRAMPOLINE_START; pointer < ((char *) MEM_USER_TRAMPOLINE_END); pointer += 0x1000){
                int64_t res = mapva(pointer, pointer, pgtbl,
                                                    NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                    NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);
                if(res < 0) return task;        
            }

            //выдели кернел стэк

            task.trapframe = (struct trapframe *) tf_page;
            task.trapframe->sp_el0 = PGHEADER(MEM_USER_STACK);
            task.trapframe->elr_el1 = (uint64_t) main;
            task.trapframe->spsr_el1 = EL0t_INTR_ON;
            task.ttbr0_el1 = (uint64_t) pgtbl;
            task.pure = true;

            return task;
}

