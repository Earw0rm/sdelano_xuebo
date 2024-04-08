#ifndef _GPIO_H
#define _GPIO_H

#include "base.h"
#include "common.h"

#define GPIO_BASE (PBASE + 0x00200000)

// alternative functuon select
#define GPFSEL0 (GPIO_BASE)
#define GPFSEL1 (GPIO_BASE + 0x04)
#define GPFSEL2 (GPIO_BASE + 0x08)
#define GPFSEL3 (GPIO_BASE + 0x0c)
#define GPFSEL4 (GPIO_BASE + 0x10)
#define GPFSEL5 (GPIO_BASE + 0x14)

//set gpio
#define GPSET0 (GPIO_BASE + 0x1c)
#define GPSET1 (GPIO_BASE + 0x20)
//clear gpio
#define GPCLR0 (GPIO_BASE + 0x28)
#define GPCLR1 (GPIO_BASE + 0x2c)

//gpio event, actual value. Низкий или высокий уровень на пине.
#define GPLEV0 (GPIO_BASE + 0x34)
#define GPLEV1 (GPIO_BASE + 0x38)


// gpio event detect status 
// обработка асинхронных событий
// чтение 1 - случилось прерывание
// запись 1 - сбросить прерывание (событие обработанно)
#define GPEDS0 (GPIO_BASE + 0x40)
#define GPEDS1 (GPIO_BASE + 0x44)
// TODO not implemented


//RISING and FALLING являются синхронными. Это означает что сигнал сэмплируется через часы и ищет определенный паттерн
//такая синхронная тактика через сэмплирование помогает избавится от глитчей

//GPIO Pin Rising Edge Detect Enable 
#define GPREN0 (GPIO_BASE + 0x4c)
#define GPREN1 (GPIO_BASE + 0x50)
//GPIO Pin falling Edge Detect Enable 
#define GPFEN0 (GPIO_BASE + 0x58)
#define GPFEN1 (GPIO_BASE + 0x5c)

//GPIO HIGH detect enable 
#define GPHEN0 (GPIO_BASE + 0x64)
#define GPHEN1 (GPIO_BASE + 0x68)

//GPIO LOW detect enable 
#define GPLEN0 (GPIO_BASE + 0x70)
#define GPLEN1 (GPIO_BASE + 0x74)

//это АСИНХРОННОЕ включение rising обнаружение
//работает быстрее чем gpren0 но имеет проблемы с глитчами
#define GPAREN0 (GPIO_BASE + 0x7c)
#define GPAREN1 (GPIO_BASE + 0x80)

// по уровню падения асинхронное
#define GPAFEN0 (GPIO_BASE + 0x88)
#define GPAFEN1 (GPIO_BASE + 0x8c)

// pull up / pull down registers
#define GPIO_PUP_PDN_CNTRL_REG0 (GPIO_BASE + 0xe4)
#define GPIO_PUP_PDN_CNTRL_REG1 (GPIO_BASE + 0xe8)
#define GPIO_PUP_PDN_CNTRL_REG2 (GPIO_BASE + 0xec)
#define GPIO_PUP_PDN_CNTRL_REG3 (GPIO_BASE + 0xf0)





struct GpioPinData{
    reg32 reserved;
    reg32 data[2]; 
};

struct GpioRegs{
    reg32 func_select[6]; // gpfsel0 .. gpfsel5
    struct GpioPinData output_set;
    struct GpioPinData output_clear;
    struct GpioPinData level;
    struct GpioPinData ev_detect_status;
    struct GpioPinData re_detect_enable;        
    struct GpioPinData fe_detect_enable;
    struct GpioPinData hi_detect_enable;
    struct GpioPinData lo_detect_enable;
    struct GpioPinData async_re_detect;
    struct GpioPinData async_fe_detect;

    reg32 reserved[21];
    reg32 pup_pdwn_cntrl[4];
};

#define TESTGPIO_REGS ((struct GpioRegs *) (TESTPBASE + 0x00200000))
// #define GPIO_REGS TESTGPIO_REGS
#define GPIO_REGS ((struct GpioRegs *) (PBASE + 0x00200000))



#endif