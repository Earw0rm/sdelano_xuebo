#include "base.h"
#include "GIC400.h"


// 0x0000-0x0FFF Reserved
// 0x1000-0x1FFF Distributor
// 0x2000-0x3FFF CPU interfaces



uint32_t get_gic400_info(void){
    return GIC400_DISTRIBUTOR->type;
}


void gic400_turn_offd(void){
    GIC400_DISTRIBUTOR->ctl = CTL_DISABLE;
}
void gic400_turn_ond(void){
    GIC400_DISTRIBUTOR->ctl = CTL_ENABLE;
}


uint32_t gic400_ask(void){
    return (GIC400_INTERFACES->ia & 0x3FFU);
}

void gic400_eoi(uint32_t interrupt_id){
    uint32_t eoi = GIC400_INTERFACES->eoi;
    eoi |= interrupt_id;
    GIC400_INTERFACES->eoi = eoi;
}

// do nothing if timer_num > 3
void gic400_enable_sys_timer(uint32_t timer_num){
    if(timer_num > 3) return; 
     // interrupt id 96 = sys_timer_0;    99 sys_timer_3
    GIC400_DISTRIBUTOR->isenable[3] = (1 << timer_num);
    // change sys_timer_n to group_1 type (non-secure)
    GIC400_DISTRIBUTOR->igroup[3] = (1 << timer_num);
}

// Distributor and local interface turned off after this function
void gic400_global_init(void){
    GIC400_DISTRIBUTOR->ctl = CTL_DISABLE;


    uint64_t it_lines_support = GIC_TYPE_ITLINESNUMBER(get_gic400_info());

    for(uint32_t i = 0; i < (it_lines_support + 1); ++i){
    
        if(i != 0){
            // configure each interrupt to be in group 1(non-secure)
            // that we can hold and handle it in EL1(operating system layer)    
            GIC400_DISTRIBUTOR->igroup[i]   = 0xffffffff;

            // clear all state of interrupts 
            GIC400_DISTRIBUTOR->icenable[i] = 0xffffffff;
            GIC400_DISTRIBUTOR->icpend[i]   = 0xffffffff;
            GIC400_DISTRIBUTOR->icactive[i] = 0xffffffff;
        }

    }

    for(uint32_t i = 0; i < (it_lines_support + 1)  * 2; ++i){
        if(i > 1){
            // using N-N models and level-sensetive
            //TODO: For SGIs, Int_config fields are read-only, meaning that GICD_ICFGR0 is read-only. 
            // GICD_ICFGR1 is banked
            GIC400_DISTRIBUTOR->icfg[i] = 0;
        }
    }


    for(uint32_t i = 0; i < (it_lines_support + 1)  * 8; ++i){
        // priority and target 
        // In a multiprocessor implementation, GICD_IPRIORITYR0 to GICD_IPRIORITYR7 are
        // banked for each connected processo
        if(i > 7){
            GIC400_DISTRIBUTOR->ipriority[i] = 0xa0a0a0a0;
            GIC400_DISTRIBUTOR->istargets[i] = 0x0f0f0f0f;
        }
    }

    // GIC400_INTERFACES->pm = 0xf0;
    // GIC400_INTERFACES->ctl = CTL_ENABLE;

}

// call this for each core
void gic400_local_init(void){
    GIC400_DISTRIBUTOR->ctl = CTL_DISABLE;
    GIC400_INTERFACES->ctl = CTL_DISABLE;

    uint64_t it_lines_support = GIC_TYPE_ITLINESNUMBER(get_gic400_info());

    GIC400_DISTRIBUTOR->igroup[0]   = 0xffffffff;
    GIC400_DISTRIBUTOR->icenable[0] = 0xffffffff;
    GIC400_DISTRIBUTOR->icpend[0]   = 0xffffffff;
    GIC400_DISTRIBUTOR->icactive[0] = 0xffffffff;
    

    // GIC400_DISTRIBUTOR->icfg[0] = 0; // read only field
    GIC400_DISTRIBUTOR->icfg[1] = 0; // for ppi

    for(uint32_t i = 0; i < 8; ++i){
        // priority and target 
        // In a multiprocessor implementation, GICD_IPRIORITYR0 to GICD_IPRIORITYR7 are
        // banked for each connected processo
        GIC400_DISTRIBUTOR->ipriority[i] = 0xa0a0a0a0;

        // GICD_ITARGETSR0 to GICD_ITARGETSR7 are read-only, and each field returns
        // a value that corresponds only to the processor reading the register.
        // • It is IMPLEMENTATION DEFINED which, if any, SPIs are statically conf
        // GIC400_DISTRIBUTOR->istargets[i] = 0x0f0f0f0f;
    }

}

void gic400_turn_offi(void){
    GIC400_INTERFACES->ctl = CTL_DISABLE;
}

void gic400_turn_oni(void){
    GIC400_INTERFACES->ctl = CTL_ENABLE;
}