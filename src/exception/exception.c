#include "exception.h"
#include "printf.h"
#include "GIC400.h"
#include "arm/arm_util.h"
#include "sys_timer.h"
#include "sched.h"
#include "sys_timer.h"
#include "arm/sysregs.h"
#include "sys.h"
#include "mini_uart.h"
#include "scheduler.h"

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




// this functiion can be call only after init_printf();
void show_invalid_entry_message(uint32_t ex_type, uint32_t esr_el1, uint32_t elr_el1){
    const char * err_msg = (entry_error_messages[ex_type]);
    printf("An exception occurred without a proper handler. \r\n Reason: %s, esr_el1: %u, elr_el1: %u. \r\n", err_msg, esr_el1, elr_el1);
}

void el3_panic_msg(uint64_t esr, uint64_t elr){
    muart_send_string("El3_panic \r\n");
    printf("[EL3] panic. esr %x, elr %x", esr, elr);
}

// In GICv2, when using a software model with the GICC_CTLR.AckCtl bit set to 0, separate registers
// are used to manage Group 0 and Group 1 interrupts, as follows:
// — GICC_IAR, GICC_EOIR, and GICC_HPPIR for Group 0 interrupts
// — GICC_AIAR, GICC_AEOIR, and GICC_AHPPIR for Group 1 interrupts
void handle_irq(void){

    uint32_t interrupt_id = gic400_ask();

    if(interrupt_id == GIC_SPURIOUS_INTR) return;

    // uint32_t current_el = get_current_el();
    //TODO define constant that can enable or disable debug msgs.
    // printf("Handle_irq. Interrupt id is: %d \r\n", interrupt_id);
    // printf("Exception level : EL%d \r\n ", current_el);

    switch (interrupt_id){
        case SYS_TIMER_0:
            break;
        case SYS_TIMER_3:
            sys_timer_recharge(3, SCHEDULER_RECHARGE_TIME);
            schedule();
            gic400_eoi(SYS_TIMER_3);
            break;
        default:
            break;
    }
}


void handle_el0_sync(uint64_t x0, uint64_t x1,
                     uint64_t x2, uint64_t x3,
                     uint64_t x4, uint64_t x5,
                     uint64_t x6, uint64_t x7,
                     uint64_t x8){

    uint64_t esr_el1 = get_exception_syndrome1();
    uint64_t reason = (esr_el1 & (0x3fU << ESR_EL1_REASON_SHIFT));

    switch (reason){
        case ESR_EL1_REASON_SVC: 
            enable_irq(); 
            handle_syscall(x0, x1, x2, x3, x4, x5, x6, x7, x8);
            break;
        default:
            // TODO someproblems 
            printf("handle_el0_sync PANIC! \r\n");
            break;
    }
}
void handle_el1h_sync(uint64_t far_el1, uint64_t esr_el1, uint64_t elr_el1){
    uint64_t syndrome = (esr_el1 & EC_MASK);

    switch (syndrome)
    {
    case EC_DATA_ABORT: //big crunch

        break;
    
    default:
        break; // big fuckin trap
    }
}