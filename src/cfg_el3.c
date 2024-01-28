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


    uint32_t it_lines_support = get_num_of_it_lines_support();
    printf("EL3 initialization is finished. GIC400 number of it_lines_support: %d \r\n. Dataframe: \r\n", it_lines_support);
    for(uint32_t i = 0; i < (it_lines_support + 1); ++i){
        uint32_t is_enabled_df = GIC400_DISTRIBUTOR->isenable[i];
        printf("%d => %u \r\n", i, is_enabled_df);
    }

}
void check(void){
    uint32_t check = 10;
    printf("Check %d", check);
}
