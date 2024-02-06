#include "exception.h"
#include "printf.h"
#include "GIC400.h"
#include "arm/arm_util.h"
#include "sys_timer.h"
#include "sched.h"
#include "sys_timer.h"
const char *entry_error_messages[] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "FIQ_INVALID_EL1t",
    "ERROR_INVALID_EL1t"

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h"

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_65",
    "FIQ_INVALID_EL0_65",
    "ERROR_INVALID_EL0_65"

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32"
};


inline static void _eoi(uint32_t interrupt_id){
    uint32_t eoi = GIC400_INTERFACES->eoi;
    eoi |= interrupt_id;
    GIC400_INTERFACES->eoi = eoi;
}
// this functiion can be call only after init_printf();
void show_invalid_entry_message(uint32_t ex_type, uint32_t esr_el1, uint32_t elr_el1){
    const char * err_msg = (entry_error_messages[ex_type]);
    printf("An exception occurred without a proper handler. \r\n Reason: %s, esr_el1: %u, elr_el1: %u. \r\n", err_msg, esr_el1, elr_el1);
}

// In GICv2, when using a software model with the GICC_CTLR.AckCtl bit set to 0, separate registers
// are used to manage Group 0 and Group 1 interrupts, as follows:
// — GICC_IAR, GICC_EOIR, and GICC_HPPIR for Group 0 interrupts
// — GICC_AIAR, GICC_AEOIR, and GICC_AHPPIR for Group 1 interrupts
void handle_irq(void){
    // reg32 aia; ask
    // reg32 aeoi; 
    uint32_t interrupt_id = GIC400_INTERFACES->ia & 0x3FFU;
    if(interrupt_id == GIC_SPURIOUS_INTR) return;


    // uint32_t current_el = get_current_el();
    //TODO define constant that can enable or disable debug msgs.
    // printf("Handle_irq. Interrupt id is: %d \r\n", interrupt_id);
    // printf("Exception level : EL%d \r\n ", current_el);

    switch (interrupt_id){
        case SYS_TIMER_0:
                /* code */
            break;
        case SYS_TIMER_3:
            sys_timer_recharge(3, SCHEDULER_RECHARGE_TIME);
            _eoi(interrupt_id);
            timer_tick();
            break;
        default:
            break;
    }
}