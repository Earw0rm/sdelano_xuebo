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
extern void putc(void* p, char c);

static bool global_initialization_is_completed = false;
static uint8_t local_initialization_completion_counter = 0;

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

        uint64_t parange = get_parange();
        uint32_t gic_type = get_gic400_info();

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

        __atomic_thread_fence(__ATOMIC_ACQUIRE);    
        /**
         *  1) check than pgtbl is not 0 
         *  2) map stack and another stuff ( addresses and another stuff) to mmu
        */
        global_initialization_is_completed = true;
        printf("[EL3]: Core_id = %x global_initialization_is_completed = %x \r \n", core_id, global_initialization_is_completed);
        
        pagetable_t pgtbl = init_mmu(core_id);
        w_ttbr1_el1((uint64_t)pgtbl);
        w_ttbr0_el1((uint64_t)pgtbl);

        while(__atomic_load_n(&local_initialization_completion_counter, __ATOMIC_RELAXED) != 3){
            asm volatile("nop");
        }

        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        gic400_turn_ond();
    }else{
        while(!global_initialization_is_completed){
            asm volatile("nop");
        }
        gic400_local_init();
        gic400_turn_oni();
        /**
         *  1) check than pgtbl is not 0 
         *  2) map stack and another stuff ( addresses and another stuff) to mmu
        */
        pagetable_t pgtbl = init_mmu(core_id);
        w_ttbr1_el1((uint64_t)pgtbl);
        w_ttbr0_el1((uint64_t)pgtbl);
    
        __atomic_add_fetch(&local_initialization_completion_counter, 1, __ATOMIC_ACQUIRE);
    }


///////////##################################################################

    // uint64_t stack0_map_res = mapva(up_of_stack0, up_of_stack0, pgtbl, NORMAL_NC);
    // uint64_t stack1_map_res = mapva(up_of_stack1, up_of_stack1, pgtbl, NORMAL_NC);

    
    volatile bool wait = false;
    while (wait);

    __atomic_thread_fence(__ATOMIC_ACQUIRE);

    // enable_mmu();
    asm volatile("eret");// Jump to kernel_main, el1h 


    printf("[EL3] PANIC! Return into start.");
    while (1);
    
}

