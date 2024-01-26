#include "base.h"
#include "GIC400.h"


// 0x0000-0x0FFF Reserved
// 0x1000-0x1FFF Distributor
// 0x2000-0x3FFF CPU interfaces

void gic400_turn_off_distributor(void){
    GIC400_DISTRIBUTOR->ctl = CTL_DISABLE;
}

int gic400_init(void){
    GIC400_DISTRIBUTOR->ctl = CTL_DISABLE;
    GIC400_INTERFACES->ctl = CTL_DISABLE;
    
    uint32_t it_lines_support = GIC400_DISTRIBUTOR->type & 0xF;

    for(uint32_t i = 0; i < (it_lines_support + 1); ++i){
        // configure each interrupt to be in group 1(non-secure)
        // that we can hold and handle it in EL1(operating system layer)
        // bit 1 = Group 1

        // GIC400_DISTRIBUTOR->igroup[i]   = 0xffffffff;
        GIC400_DISTRIBUTOR->icenable[i] = 0xffffffff;
        GIC400_DISTRIBUTOR->icpend[i]   = 0xffffffff;
        GIC400_DISTRIBUTOR->icactive[i] = 0xffffffff;


        //do something with that shit

    }

    for(uint32_t i = 0; i < (it_lines_support + 1)  * 2; ++i){
        if(i == 0){

        }else{
            // using N-N models and level-sensetive
            GIC400_DISTRIBUTOR->icfg[i] = 0;
        }
    }
    

    // TODOOO
    for(uint32_t i = 0; i < (it_lines_support + 1)  * 8; ++i){
        // priority and target 
        GIC400_DISTRIBUTOR->ipriority[i] = 0xa0a0a0a0;
        GIC400_DISTRIBUTOR->istargets[i] = 0x01010101;
    }

    GIC400_DISTRIBUTOR->ctl = CTL_ENABLE;
    GIC400_INTERFACES->pm = 0xf0;
    GIC400_INTERFACES->ctl = CTL_ENABLE;
    return 0;
}
void gic400_enable_sys_timer(uint32_t timer_num){
    GIC400_DISTRIBUTOR->ctl = CTL_DISABLE;
    GIC400_DISTRIBUTOR->isenable[3] = (1 << timer_num); // interrupt id 96 = sys_timer_0;    99 sys_timer_3
    GIC400_DISTRIBUTOR->ctl = CTL_ENABLE;
}