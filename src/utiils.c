#include "utils.h"



void delay(uint32_t tick){
    for(uint32_t i = 0; i < tick; ++i){
        asm volatile("nop");
    }
}