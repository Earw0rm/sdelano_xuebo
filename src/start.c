#include "GIC400.h"
#include "sys_timer.h"
#include "arm/arm_util.h"
#include "printf.h"
#include "mini_uart.h"
#include "pa_alloc.h"
#include "arm/sysregs.h"
#include "sched.h"

__attribute__((aligned(16)))
volatile char init_stack3[4096 * 4]; 

__attribute__((aligned(16)))
volatile char kernel_stack1[4096 * 4];

extern char vectors[]; // exception.S
extern char el3_vec[]; // exception.S

extern void kernel_main(void);
extern void putc(void* p, char c); // why do we need this?


//sry for that
static bool global_initialization_is_completed = false;
static const bool global_initialization_completed = true; // for atomic store

static uint8_t local_initialization_completion_counter = 0;
static uint8_t eret_barrier = 0;

static const bool atomic_test = true;

// PBASE depend on this variable;
bool configuration_is_completed = 0;


void configure_el3(uint64_t core_id){
    
    w_vbar_el3((uint64_t) &el3_vec);
    w_sctlr_el1(SCTLR_VALUE_MMU_DISABLED);
    w_tcr_el1(TCR_VALUE);
    w_mair_el1(MAIR_VALUE);
    w_elr_el3((uint64_t)  (((uint64_t) &kernel_main) )); // | VAKERN_BASE 
    w_vbar_el1((uint64_t) (((uint64_t) &vectors) )); //| VAKERN_BASE

    uint64_t stack1_addr = (uint64_t) &kernel_stack1[((core_id + 1) << 12)];
    w_sp_el1(stack1_addr);

    if(core_id == 0){ // TODO AFTER ALL CONFIGURATION TURN ON DISTRIBUTOR 

        muart_init();
        init_printf(0, unsafe_putc);
        printf("[EL3]: Welcome. Core_id = %x \r \n", core_id);

        // uint64_t parange = get_parange();
        // uint32_t gic_type = get_gic400_info();

        w_hcr_el2(HCR_VALUE);
        w_scr_el3(SCR_VALUE);
        w_spsr_el3(SPSR_VALUE);


        gic400_global_init();
        gic400_local_init();
        gic400_turn_oni();

        sys_timer_init();
        gic400_enable_sys_timer(3); //todo peredelat'


        //phisycal allocator
        // todo for local test
        uint64_t num_of_init_pages = init_pa_alloc();
        uint64_t pages_after_init = get_num_of_free_pages();

        //init_task_initialization(up_of_stack0, up_of_stack1); We need another pointer to the stack in multiprocessor systems

        uint8_t init_res = kpgtbl_init(); // check
        w_ttbr1_el1((uint64_t)&kpgtbl);


        /**
         *  1) check than pgtbl is not 0 
         *  2) map stack and another stuff ( addresses and another stuff) to mmu
        */
       __atomic_store(&global_initialization_is_completed, &global_initialization_completed, __ATOMIC_ACQ_REL);

        
        while(__atomic_load_n(&local_initialization_completion_counter, __ATOMIC_ACQUIRE) != 3){
            asm volatile("nop");
        }

        gic400_turn_ond();

        __atomic_thread_fence(__ATOMIC_RELEASE);
    }else{
        while(!global_initialization_is_completed){
            asm volatile("nop");
        }
        gic400_local_init();
        gic400_turn_oni();
        /**

         *  2) map stack and another stuff ( addresses and another stuff) to mmu
        */
        w_ttbr1_el1((uint64_t)&kpgtbl);

    
        __atomic_add_fetch(&local_initialization_completion_counter, 1, __ATOMIC_ACQ_REL);
    }


///////////##################################################################

    // uint64_t stack0_map_res = mapva(up_of_stack0, up_of_stack0, pgtbl, NORMAL_NC);
    // uint64_t stack1_map_res = mapva(up_of_stack1, up_of_stack1, pgtbl, NORMAL_NC);

    
    volatile bool wait = false;
    while (wait);

    //IM A GOD OF MULTICORE PROGRAMMING WITHOUT A LOCK
    if(__atomic_add_fetch(&eret_barrier, 1, __ATOMIC_RELEASE) == 4){
        configuration_is_completed = 1;
    }
    
    while(__atomic_load_n(&eret_barrier, __ATOMIC_ACQUIRE) != 4){
            asm volatile("nop");
    }

    enable_mmu();
    
    asm volatile("isb");
    asm volatile("eret");// Jump to kernel_main, el1h 


    printf("[EL3] PANIC! Return into start.");
    while (1);
    
}

