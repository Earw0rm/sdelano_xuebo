
#ifndef _MINI_UART_H
#define _MINI_UART_H

#include "base.h"
#include "common.h"

/**
 * MU - mini uart.
 * AUX_ANBLES: bit 0 -> muart, bit 1 -> spi 1, bit 2 -> spi 2
 * GPIO pins should be set up first before enabling the UART. 
 * The UART core is built to emulate 16550 behaviour. 
 * Immediately after a reset, the baudrate register will be zero and the system clock will be 250 MHz
*/
#define AUX_ENABLES     (PBASE+0x00215004) 
#define AUX_MU_IO_REG   (PBASE+0x00215040) // IO data 
#define AUX_MU_IER_REG  (PBASE+0x00215044) // interrupt enable 
#define AUX_MU_IIR_REG  (PBASE+0x00215048) // interruot identify
#define AUX_MU_LCR_REG  (PBASE+0x0021504C) // line control 
#define AUX_MU_MCR_REG  (PBASE+0x00215050) // modem control
#define AUX_MU_LSR_REG  (PBASE+0x00215054) // line status
#define AUX_MU_MSR_REG  (PBASE+0x00215058) // modem status
#define AUX_MU_SCRATCH  (PBASE+0x0021505C) // scratch 
#define AUX_MU_CNTL_REG (PBASE+0x00215060) // extra control
#define AUX_MU_STAT_REG (PBASE+0x00215064) // extra status
#define AUX_MU_BAUD_REG (PBASE+0x00215068) // baudrate


#define AUX_UART_CLOCK (250000000)
#define AUX_MU_BAUD(baud) ((AUX_UART_CLOCK/(baud*8))-1)


struct AuxRegs{
    reg32 aux_irq;
    reg32 aux_enables;
    reg32 reserved[14];
    reg32 aux_mu_io_reg;
    reg32 aux_mu_ier_reg;
    reg32 aux_mu_iir_reg;
    reg32 aux_mu_lcr_reg;
    reg32 aux_mu_mcr_reg;
    reg32 aux_mu_lsr_reg;
    reg32 aux_mu_msr_reg;
    reg32 aux_mu_scratch;
    reg32 aux_mu_cntl_reg;
    reg32 aux_mu_stat_reg;
    reg32 aux_mu_baud_reg;
};

#define REGS_AUX ((struct AuxRegs *)(PBASE+0x00215000))

// txd1 & rxd1 это auxilary I/O. На gpio надо включить что бы mini uart работал. 
// gpio 14 and 15 must be SEL 5
void muart_init(void);

char muart_recv(void);
void muart_send(char c);
void muart_send_string(char* str);



//for printf library
void putc(void* p, char c);


#endif