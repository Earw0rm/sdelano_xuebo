#include "scheduler.h"
#include "arm/arm_util.h"
#include "speenlock.h"
#include "vm.h"
#include "pa_alloc.h"
#include "memlayout.h"
#include "GIC400.h"
#include "exception.h"
#include "printf.h"


__attribute__((section(".thread_shared")))
static struct speenlock tasks_lock = {
    .cpu_num = -1,
    .locked = 0,
    .name = "clear"
};


//current task live hire 
__attribute__((aligned(0x1000)))
volatile char cpus[0x4000]   = {0};
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
    uint64_t addr = LAYOUT_MY_CPU(get_processor_id());

    return (struct cpu *) addr;
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

            uint64_t stack_page = get_page();
            zero_range((char *)stack_page, 0x1000);


            int64_t mapva_res;

            mapva_res = mapva(MEM_USER_TRAPFRAME, tf_page, pgtbl,
                                                NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);

            mapva_res = mapva(MEM_USER_STACK, stack_page, pgtbl,
                                                NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);                                                             

            for(char * pointer = (char *) MEM_USER_TRAMPOLINE_START; pointer < ((char *) MEM_USER_TRAMPOLINE_END); pointer += 0x1000){
                

                mapva_res = mapva((uint64_t)pointer, (uint64_t)pointer, pgtbl,
                                                    NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                    NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);
                if(mapva_res < 0) return task;        
            }



            //map for user code 
            for(char * pointer = (char *) MEM_USER_START;
                       pointer < ((char *) (MEM_USER_START + 0x5000)); 
                       pointer += 0x1000, 
                       main    += 0x1000){
                    
                mapva_res = mapva((uint64_t)pointer,(uint64_t)main, pgtbl,
                                                    NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                    NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);
           

                if(mapva_res < 0) return task;        
            }


            task.trapframe = (struct trapframe *) tf_page;
            task.trapframe->sp_el0 = PGHEADER(MEM_USER_STACK);
            task.trapframe->spsr_el1 = EL0t_INTR_ON;
            task.ttbr0_el1 = (uint64_t) pgtbl;


            task.trapframe->elr_el1 = MEM_USER_START;
            task.pure = true;

            return task;
}

struct task user_task_create2(uint8_t (*main)(void), pagetable_t pgtbl){
            struct task task = {0};

            // pagetable_t pgtbl = (pagetable_t) get_page();
            // zero_range((char *)pgtbl, 0x1000);

            uint64_t tf_page = get_page();
            zero_range((char *)tf_page, 0x1000);

            uint64_t stack_page = get_page();
            zero_range((char *)stack_page, 0x1000);


            int64_t mapva_res;

            mapva_res = mapva(MEM_USER_TRAPFRAME, tf_page, pgtbl,
                                                NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);

            mapva_res = mapva(MEM_USER_STACK, stack_page, pgtbl,
                                                NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);                                                             

            for(char * pointer = (char *) MEM_USER_TRAMPOLINE_START; pointer < ((char *) MEM_USER_TRAMPOLINE_END); pointer += 0x1000){
                

                mapva_res = mapva((uint64_t)pointer, (uint64_t)pointer, pgtbl,
                                                    NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                    NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);
                if(mapva_res < 0) return task;        
            }



            //map for user code 
            for(char * pointer = (char *) MEM_USER_START;
                       pointer < ((char *) (MEM_USER_START + 0x5000)); 
                       pointer += 0x1000, 
                       main    += 0x1000){
                    
                mapva_res = mapva((uint64_t)pointer,(uint64_t)main, pgtbl,
                                                    NORMAL_IO_WRITE_BACK_RW_ALLOCATION_TRAINSIENT,
                                                    NON_SHAREABLE, VALID_DESCRIPTOR | PAGE_DESCRIPTOR, false);
           

                if(mapva_res < 0) return task;        
            }


            task.trapframe = (struct trapframe *) tf_page;
            task.trapframe->sp_el0 = PGHEADER(MEM_USER_STACK);
            task.trapframe->spsr_el1 = EL0t_INTR_ON;
            task.ttbr0_el1 = (uint64_t) pgtbl;


            task.trapframe->elr_el1 = MEM_USER_START;
            task.pure = true;

            return task;
}



void init_task(uint8_t (*main)(void)){
    disable_irq();
    

    bool wait = true;
    while(wait);
    struct task task = user_task_create(main);

    my_cpu()->current_task = task;
    
    print_pgtbl((pagetable_t) task.ttbr0_el1);

    ((void (*) (void))(VAKERNBASE | (uint64_t) &el0_irq_ret))();
}