#include "GIC400.h"
#include "sys_timer.h"
#include "arm/arm_util.h"
#include "printf.h"
#include "mini_uart.h"
#include "pa_alloc.h"
#include "arm/sysregs.h"
#include "sched.h"

__attribute__((aligned(16))) char init_stack3[4096 * 4];

extern char vectors[]; // exception.S
extern char el3_vec[]; // exception.S
extern void kernel_main(void);
extern void putc(void* p, char c);



void configure_el3(void){


    muart_init();

    w_vbar_el3((uint64_t) &el3_vec);
    init_printf(0, putc);
    printf("[EL3]: Configuration start. \r \n");

    gic400_init();
    sys_timer_init();
    gic400_enable_sys_timer(3);

    w_sctlr_el1(SCTLR_VALUE_MMU_DISABLED);
    w_hcr_el2(HCR_VALUE);
    w_scr_el3(SCR_VALUE);
    w_spsr_el3(SPSR_VALUE);


   




    // todo for local test
    uint64_t num_of_init_pages = init_pa_alloc();
    uint64_t pages_after_init = get_num_of_free_pages();


    uint64_t itstack0 = get_page();
    uint64_t itstack1 = get_page();
    

    if(itstack0 == 0 || itstack1 == 0){
        printf("[EL3]: PANIC! itstack0: %d; itstack1: %d  \r \n", itstack0, itstack1);
        return;
    }


    itstack0 = PAGE_UP(itstack0) | VAKERN_BASE;
    itstack1 = PAGE_UP(itstack1) | VAKERN_BASE;

    w_elr_el3((uint64_t)  (((uint64_t) &kernel_main) | VAKERN_BASE)); 
    w_vbar_el1((uint64_t) (((uint64_t) &vectors) | VAKERN_BASE));

    w_sp_el0(itstack0);
    w_sp_el1(itstack1);

    init_task_initialization(itstack0, itstack1);

    pagetable_t pgtbl = init_mmu();

    
    if(pgtbl == 0){
        printf("[EL3]: PANIC! MMU initialization fail.");
        return;
    }

    w_ttbr1_el1((((uint64_t) pgtbl) | VAKERN_BASE));

    asm volatile("isb");


    if(!init_task_is_initialized){
        printf("[EL3]: PANIC! Init task is not initialized. \r\n");
        return;
    }


    printf("[EL3]: Register settings is completed. Read value of system registers and print: \r \n");

    printf("[EL3]: sctlr_el1 value: %x. \r \n", r_sctlr_el1());
    printf("[EL3]: hcr_el2 value: %x. \r \n", r_hcr_el2());
    printf("[EL3]: scr_el3 value: %x. \r \n", r_scr_el3());
    printf("[EL3]: spsr_el3 value: %x. \r \n", r_spsr_el3());
    printf("[EL3]: elr_el3 value addres: %x. \r \n", r_elr_el3());
    printf("[EL3]: vbar_el1 vector addres: %x. \r \n", r_vbar_el1());
    printf("[EL3]: vbar_el3 vector addr: %X. \r \n", r_vbar_el3());

    printf("[EL3]: sp_el0 addr: %X. \r \n", r_sp_el0());
    printf("[EL3]: sp_el1 addr: %X. \r \n", r_sp_el1());


    // some local boolsheet tests
    printf("[EL3]: Num of init free pages : %d. \r \n", num_of_init_pages);
    printf("[EL3]: Num of free pages after get : %d. \r \n", pages_after_init);

    printf("[EL3]: Configuration is completed. Jump to kernel main. \r \n");
    
    asm volatile("eret"); // Jump to kernel_main, el1h 
    printf("[EL3] PANIC! Return into start.");
    while (1);
    
}

