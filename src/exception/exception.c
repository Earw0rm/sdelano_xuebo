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






void el3_panic_msg(uint64_t esr, uint64_t elr){
    muart_send_string("El3_panic \r\n");
    printf("[EL3] panic. esr %x, elr %x", esr, elr);
}

// In GICv2, when using a software model with the GICC_CTLR.AckCtl bit set to 0, separate registers
// are used to manage Group 0 and Group 1 interrupts, as follows:
// — GICC_IAR, GICC_EOIR, and GICC_HPPIR for Group 0 interrupts
// — GICC_AIAR, GICC_AEOIR, and GICC_AHPPIR for Group 1 interrupts
void handle_irq_el0(void){

    uint32_t interrupt_id = gic400_ask();

    if(interrupt_id == GIC_SPURIOUS_INTR) return;

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

void handle_irq_el1(void){

    uint32_t interrupt_id = gic400_ask();

    if(interrupt_id == GIC_SPURIOUS_INTR) return;

    switch (interrupt_id){
        case SYS_TIMER_0:
            break;
        case SYS_TIMER_3:
            sys_timer_recharge(3, SCHEDULER_RECHARGE_TIME);
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

    uint64_t esr_el1 = r_esr_el1();
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
void handle_el1h_sync(void){
    uint64_t esr_el1 = r_esr_el1();

    uint64_t syndrome = (esr_el1 & EC_MASK);

    switch (syndrome){
        case EC_DATA_ABORT: //big crunch
            uint64_t far_el1;
            break;
        
        default:
            break; // big fuckin trap
    }
}