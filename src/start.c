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
volatile char init_stack3[0x4000]  = {0}; 

extern char vectors[]; // exception.S
extern char el3_vec[]; // exception.S

extern void kernel_main(void);

static bool global_initialization_is_completed_el3 = false;

__attribute__((section(".thread_shared")))
static bool global_initialization_is_completed_el1 = false;

static const bool completed = true;


void configure_el1(void){
    uint64_t core_id = get_processor_id();
    
    if(core_id == 0){

        muart_init();
        init_printf(0, putc); 
        
        gic400_global_init();

        // todo change to local timer 
        sys_timer_init();
        gic400_enable_sys_timer(3); 

       __atomic_store(&global_initialization_is_completed_el1, &completed, __ATOMIC_RELEASE);
    }else{
        while(!__atomic_load_n(&global_initialization_is_completed_el1, __ATOMIC_ACQUIRE)){
            asm volatile("nop");
        }
    }

    gic400_local_init();
    gic400_turn_oni();

    //todo tmp
    disable_irq();
    __atomic_thread_fence(__ATOMIC_ACQ_REL);

    if(core_id == 0){
        gic400_turn_ond();
    }

    kernel_main();

    while(1);
}

void configure_el3(uint64_t core_id){


    w_vbar_el3((uint64_t) &el3_vec);
    w_sctlr_el1(SCTLR_VALUE_MMU_DISABLED_CACHE_ENABLED);
    w_tcr_el1(TCR_VALUE);
    w_mair_el1(MAIR_VALUE);
    

    w_elr_el3(((uint64_t) &configure_el1)); //  
    w_vbar_el1((uint64_t) &vectors); 
    w_hcr_el2(HCR_VALUE);
    w_scr_el3(SCR_VALUE);

    //stuff before interrupt
    w_spsr_el3(SPSR_VALUE);
    w_sp_el1(PGHEADER(KSTACK(core_id)));

    if(core_id == 0){

        uint64_t num_of_init_pages = init_pa_alloc();


        int8_t init_res = kpgtbl_init();
  
       __atomic_store(&global_initialization_is_completed_el3, &completed, __ATOMIC_RELEASE);
    }else{
        while(!__atomic_load_n(&global_initialization_is_completed_el3, __ATOMIC_ACQUIRE)){
            asm volatile("nop");
        }
    }

    w_ttbr0_el1((uint64_t)&kpgtbl);
    w_ttbr1_el1((uint64_t)&kpgtbl);

    enable_mmu();


    asm volatile("isb");
    asm volatile("eret");// Jump to configure_el1, el1h 
    while(1);
}

