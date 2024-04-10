#include "scheduler.h"
#include "arm/arm_util.h"
#include "speenlock.h"
#include "vm.h"
#include "pa_alloc.h"

__attribute__((section(".data.thread_shared")))
static struct speenlock tasks_lock = {
    .cpu_num = -1,
    .locked = 0,
    .name = "clear"
};

static struct cpu cpus[4]    = {0};
static uint64_t tasks_count  =  0 ;
static struct task tasks[64] = {0};



struct cpu my_cpu(void){
    return cpus[get_processor_id()];
}

void schedule(void){
    //todo implement this
}

struct task create_task(void){
    uint64_t sp_el0_page = get_page();
    uint64_t sp_el1_page = get_page();

    uint64_t ttbr0_page = get_page();
    uint64_t ttbr1_page = get_page();

    zero_range((char *)sp_el0_page, 0x1000);
    zero_range((char *)sp_el1_page, 0x1000);
    zero_range((char *)ttbr0_page,  0x1000);
    zero_range((char *)ttbr1_page,  0x1000);            

    uint8_t init_res = kpgtbl_init((pagetable_t) ttbr1_page);
    
}