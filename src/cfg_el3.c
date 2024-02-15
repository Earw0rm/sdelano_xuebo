#include "arm/cfg_el3.h"
#include "GIC400.h"
#include "sys_timer.h"
#include "arm/arm_util.h"
#include "printf.h"
#include "mini_uart.h"
#include "pa_alloc.h"


void configure_el3(void){
    muart_init();
    init_printf(0, putc);
    printf("[EL3]: Miniuart & printf initialization is finished. Starting initialization of EL3.\r\n");
    gic400_init();
    sys_timer_init();
    gic400_enable_sys_timer(3);
    uint64_t num_of_free_pages_at_init = init_pa_alloc();
    printf("[EL3]: Init PA pages complete. Num of free pages: %d. \r\n", num_of_free_pages_at_init);
    printf("[EL3]: End of kernel address: %u. \r\n", PA_KERNEL_END);
    printf("[EL3]: EL3 initialization is finished.\r\n");
}

