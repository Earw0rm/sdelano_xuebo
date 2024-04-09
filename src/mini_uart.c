#include "mini_uart.h"
#include "common.h"
#include "gpio.h"
#include "speenlock.h"


__attribute__((section(".data.thread_shared")))
static struct speenlock uart_lock = {
    .cpu_num = -1,
    .locked = 0,
    .name = "clear"
};



void muart_init(void){
    uint32_t sel = GPIO_REGS->func_select[1];
    
    // FLES 14 [12:14], FSEL 15 [15:17]
    sel &= (~(7 << 12) | (~(7 << 15))); // clean 3 bit [12:14] [15:17]
    sel |= ((2 << 12) | (2 << 15)); // set alternative 5 
    GPIO_REGS->func_select[1] = sel;


   // select NO pull_up/pull_down for 14 and 15 pins
    sel = GPIO_REGS->pup_pdwn_cntrl[0]; //get32(GPIO_PUP_PDN_CNTRL_REG0);
    sel &= ((~(3 << 30)) | (~(3 << 28))); 
    GPIO_REGS->pup_pdwn_cntrl[0] = sel;


    // turn off all possible interrupts for 14 and 15 pins
    sel = GPIO_REGS->re_detect_enable.data[0];
    sel &= ((~(1 << 14)) | (~(1 << 15)));
    GPIO_REGS->re_detect_enable.data[0] = sel;

    sel = GPIO_REGS->fe_detect_enable.data[0];
    sel &= ((~(1 << 14)) | (~(1 << 15)));
    GPIO_REGS->fe_detect_enable.data[0] = sel;

    sel = GPIO_REGS->hi_detect_enable.data[0];
    sel &= ((~(1 << 14)) | (~(1 << 15)));
    GPIO_REGS->hi_detect_enable.data[0] = sel;

    sel = GPIO_REGS->lo_detect_enable.data[0];
    sel &= ((~(1 << 14)) | (~(1 << 15)));
    GPIO_REGS->lo_detect_enable.data[0] = sel;

    sel = GPIO_REGS->async_re_detect.data[0];
    sel &= ((~(1 << 14)) | (~(1 << 15)));
    GPIO_REGS->async_re_detect.data[0] = sel;

    sel = GPIO_REGS->async_fe_detect.data[0];
    sel &= ((~(1 << 14)) | (~(1 << 15)));
    GPIO_REGS->async_fe_detect.data[0] = sel;



    // enable access to AUX registers
    REGS_AUX->aux_enables = 1;

    // disable tx/rx and other stuff (RTS CTS) 
    REGS_AUX->aux_mu_cntl_reg = 0;

    // disable interrupts 
    REGS_AUX->aux_mu_ier_reg = 0;

    sel = REGS_AUX->aux_mu_iir_reg;
    sel &= (~(3 << 1));
    REGS_AUX->aux_mu_iir_reg = sel;

    REGS_AUX->aux_mu_lcr_reg = 3; // 8 bit cfg
    REGS_AUX->aux_mu_mcr_reg = 0; // set line HIGH

    // set baud rate
    REGS_AUX->aux_mu_baud_reg = AUX_MU_BAUD(115200);


    //finaly enable muart 
    REGS_AUX->aux_mu_cntl_reg = 3;
}
//TODO NOT SAFE USE THIS FUNCTION IN CONCURRENT MODE 

void muart_send(char c){

    while (1){

        uint32_t line_status = REGS_AUX->aux_mu_lsr_reg & (1 << 5);
        if(line_status) break;

    }

    REGS_AUX->aux_mu_io_reg = ((uint32_t) c);

}

// char muart_recv(void){
//     while (1){
//         uint32_t line_status = get32(AUX_MU_LSR_REG);
//         line_status &= 1;
//         if(line_status) break;
//     }

//     uint32_t res = get32(AUX_MU_IO_REG);
//     res &= 0xff;

//     return res;
// }

void muart_send_string(char* str){
    for(int i = 0; str[i] != '\0'; i ++){
        muart_send((char) str[i]);
    }
}




void putc(void* p, char c){
    muart_send(c);
}

void unsafe_putc(void* p, char c){
    muart_send(c);
}