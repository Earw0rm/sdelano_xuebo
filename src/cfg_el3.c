#include "arm/cfg_el3.h"
#include "GIC400.h"
#include "sys_timer.h"
#include "arm/arm_util.h"
#include "printf.h"
#include "mini_uart.h"



void configure_el3(void){
    muart_init();
    init_printf(0, putc);
    gic400_init();
    sys_timer_init();
    gic400_enable_sys_timer(3);



    printf("EL3 initialization is finished");
}

void kpgdentry_init(uint64_t tbl_adr, uint64_t va){

}