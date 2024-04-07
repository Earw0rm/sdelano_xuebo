#include "GIC400.h"
#include "sys_timer.h"
#include "arm/arm_util.h"
#include "printf.h"
#include "mini_uart.h"
#include "pa_alloc.h"
#include "arm/sysregs.h"
#include "sched.h"
#include "vm.h"


__attribute__((aligned(16)))
volatile char init_stack3[0x4000]; 

__attribute__((aligned(16)))
volatile char kernel_stack1[0x4000];

extern char vectors[]; // exception.S
extern char el3_vec[]; // exception.S

extern void kernel_main(void);

static bool global_initialization_is_completed_el3 = false;
static bool global_initialization_is_completed_el1 = false;





void configure_el1(void){
    uint64_t core_id = get_processor_id();
    
    if(core_id == 0){
        muart_init();
        init_printf(0, (VAKERN_BASE | ((uint64_t ) &unsafe_putc))); // temp unsafe
        
        gic400_global_init();
        sys_timer_init();
        // gic400_enable_sys_timer(3); //todo peredelat'

        const bool completed = true;
       __atomic_store(&global_initialization_is_completed_el1, &completed, __ATOMIC_RELEASE);
        // kpgtbl_debug_print(&kpgtbl);
    }else{
        while(!__atomic_load_n(&global_initialization_is_completed_el1, __ATOMIC_ACQUIRE)){
            asm volatile("nop");
        }
    }
    gic400_local_init();
    gic400_turn_oni();
    __atomic_thread_fence(__ATOMIC_ACQ_REL);
    if(core_id == 0){
        gic400_turn_ond();
    }

    kernel_main();

    while(1);
}

void configure_el3(uint64_t core_id){


    w_vbar_el3((uint64_t) &el3_vec);
    w_sctlr_el1(SCTLR_VALUE_MMU_DISABLED);
    w_tcr_el1(TCR_VALUE);
    w_mair_el1(MAIR_VALUE);


    w_elr_el3((uint64_t)  ( VAKERN_BASE | ((uint64_t) &configure_el1) )); //  
    w_vbar_el1((uint64_t) ( VAKERN_BASE | ((uint64_t) &vectors)     )); //| VAKERN_BASE

    uint64_t stack1_addr = (uint64_t) &kernel_stack1[((core_id + 1) << 12)];
    w_sp_el1(VAKERN_BASE | stack1_addr);

    if(core_id == 0){
        
        uint64_t num_of_init_pages = init_pa_alloc();
        uint64_t pages_after_init = get_num_of_free_pages();
        __atomic_thread_fence(__ATOMIC_ACQ_REL);

        w_hcr_el2(HCR_VALUE);
        w_scr_el3(SCR_VALUE);
        w_spsr_el3(SPSR_VALUE);

        
  
        uint8_t init_res = kpgtbl_init(); // check


        const bool completed = true;
       __atomic_store(&global_initialization_is_completed_el3, &completed, __ATOMIC_RELEASE);
    }else{
        while(!__atomic_load_n(&global_initialization_is_completed_el3, __ATOMIC_ACQUIRE)){
            asm volatile("nop");
        }
    }

    w_ttbr1_el1((uint64_t)&kpgtbl);
    enable_mmu();

    bool wait = true;
    while(wait);

    asm volatile("isb");
    asm volatile("eret");// Jump to kernel_main, el1h 
    while(1);
}

