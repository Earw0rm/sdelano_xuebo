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
extern void putc(void* p, char c); // why do we need this?


//sry for that
static bool global_initialization_is_completed = false;
static uint8_t local_initialization_completion_counter = 0;




// PBASE depend on this variable;
bool configuration_is_completed = false;


void configure_el3(uint64_t core_id){
    
    w_vbar_el3((uint64_t) &el3_vec);
    w_sctlr_el1(SCTLR_VALUE_MMU_DISABLED);
    w_tcr_el1(TCR_VALUE);
    w_mair_el1(MAIR_VALUE);


    w_elr_el3((uint64_t)  ( VAKERN_BASE | ((uint64_t) &kernel_main) )); //  
    w_vbar_el1((uint64_t) ( VAKERN_BASE | ((uint64_t) &vectors)     )); //| VAKERN_BASE

    uint64_t stack1_addr = (uint64_t) &kernel_stack1[((core_id + 1) << 12)];
    w_sp_el1(VAKERN_BASE | stack1_addr);

    if(core_id == 0){
        w_tcr_el3(TCR_EL3_VALUE);
        w_mair_el3(MAIR_VALUE);

        uint64_t num_of_init_pages = init_pa_alloc();
        uint64_t pages_after_init = get_num_of_free_pages();

        machine_pgtbl_init();
        
        volatile bool wait = true; // debug wait
        while (wait);

        w_ttbr0_el3((uint64_t)&machine_pgtbl);
        enable_mmu_el3();


        muart_init();
        init_printf(0, unsafe_putc);
        kpgtbl_debug_print((pagetable_t)&kpgtbl);

        w_hcr_el2(HCR_VALUE);
        w_scr_el3(SCR_VALUE);
        w_spsr_el3(SPSR_VALUE);

        gic400_global_init();
        gic400_local_init();
        gic400_turn_oni();

        sys_timer_init();
        gic400_enable_sys_timer(3); //todo peredelat'


        //init_task_initialization(up_of_stack0, up_of_stack1); We need another pointer to the stack in multiprocessor systems

        uint8_t init_res = kpgtbl_init(); // check
        w_ttbr1_el1((uint64_t)&kpgtbl);
        


        kpgtbl_debug_print((pagetable_t)&kpgtbl);

        /**
         *  1) check than pgtbl is not 0 
         *  2) map stack and another stuff ( addresses and another stuff) to mmu
        */
        const bool completed = true;

       __atomic_store(&global_initialization_is_completed, &completed, __ATOMIC_RELEASE);

        while(__atomic_load_n(&local_initialization_completion_counter, __ATOMIC_ACQUIRE) != 3){
            asm volatile("nop");
        }

        gic400_turn_ond();
        __atomic_store(&configuration_is_completed, &completed, __ATOMIC_RELEASE);
    }else{
        while(!__atomic_load_n(&global_initialization_is_completed, __ATOMIC_ACQUIRE)){
            asm volatile("nop");
        }
        w_ttbr0_el3((uint64_t)&machine_pgtbl);
        enable_mmu_el3();

        gic400_local_init();
        gic400_turn_oni();
        
        w_ttbr1_el1((uint64_t)&kpgtbl);
        __atomic_add_fetch(&local_initialization_completion_counter, 1, __ATOMIC_RELEASE);
    }


///////////##################################################################

    // uint64_t stack0_map_res = mapva(up_of_stack0, up_of_stack0, pgtbl, NORMAL_NC);
    // uint64_t stack1_map_res = mapva(up_of_stack1, up_of_stack1, pgtbl, NORMAL_NC);





    while(__atomic_load_n(&configuration_is_completed, __ATOMIC_ACQUIRE) != true){
        asm volatile("nop");
    }
    __atomic_thread_fence(__ATOMIC_RELEASE);

    enable_mmu();



    asm volatile("isb");
    asm volatile("eret");// Jump to kernel_main, el1h 


    printf("[EL3] PANIC! Return into start.");
    while (1);
    
}

