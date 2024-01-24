#include "arm/cfg_el3.h"
#include "GIC400.h"


void configure_el3(void){
    gic400_init();
    return;
}
