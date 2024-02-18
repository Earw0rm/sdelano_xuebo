
#include "GIC400.h"
#include "sys_timer.h"
#include "arm/arm_util.h"
#include "printf.h"
#include "mini_uart.h"
#include "pa_alloc.h"
#include "arm/sysregs.h"


__attribute__((aligned(16))) char init_stack3[4096 * 4];

extern char vectors[]; // exception.S
extern void kernel_main(void);

void configure_el3(void){
    muart_init();
    muart_send_string("this is muart send string \r\n");
    muart_send_string("this is the SECOND call of muart_send_string \r\n");
    // init_printf(0, putc);

    // muart_send_string("now we are init printf and call muart_send_string \r\n");
    // volatile int a = 10;
    // printf("[EL3]: Miniuart & printf initialization is finished. Starting initialization of EL3. %d \r\n", a);
    // muart_send_string("this is call after printf function \r\n");

    gic400_init();
    sys_timer_init();
    gic400_enable_sys_timer(3);
    // uint64_t num_of_free_pages_at_init = init_pa_alloc();
    // printf("[EL3]: Init PA pages complete. Num of free pages: %d. \r\n", num_of_free_pages_at_init);
    // printf("[EL3]: End of kernel address: %u. \r\n", PA_KERNEL_END);
    // printf("[EL3]: EL3 peripheral initialization is finished. Stariting initialization of system components. \r\n");

    w_sctlr_el1(SCTLR_VALUE_MMU_DISABLED);
    w_hcr_el2(HCR_VALUE);
    w_scr_el3(SCR_VALUE);
    w_spsr_el3(SPSR_VALUE);

    w_elr_el3((uint64_t) &kernel_main);
    w_vbar_el1((uint64_t) &vectors);
    muart_send_string("AAAAAAAAAAAAAAAAAAAAA\r\n");

    asm volatile("eret");
}

