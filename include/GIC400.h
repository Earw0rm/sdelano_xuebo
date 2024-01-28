#ifndef _GIC400_H
#define _GIC400_H

#include "common.h"
#include "base.h"
//
// (0 - 15)  interrupt id -> SGI
// (16 - 31) interrupt id -> PPI
// (32+)     interrupt id -> SPI 
//
//



// 3.3 section of CoreLink GIC-400 generic interrupt controller technical reference manual 
// 0x0000-0x0FFF Reserved
// 0x1000-0x1FFF Distributor
// 0x2000-0x3FFF CPU interfaces
// 0x4000-0x4FFF Virtual interface control block, for the processor that is performing the access
// 0x5000-0x5FFF Virtual interface control block, for the processor selected by address bits [11:9]
// 0x6000-0x7FFF Virtual CPU interfaces
//
// RPI4 Note The GIC-400 is configured with "NUM_CPUS=4" and "NUM_SPIS=192".
// 216 -> последнтй interrupt id 
// 0-15 id -> SGI. 16-31 -> PPI. 
//**
// gic_architecture_specification 35 page
// general interrupt flow 
//**

//GICD_CTRLR
#define CTL_ENABLE_GRP_0  (1 << 0)
#define CTL_ENABLE_GRP_1  (2 << 0)
#define CTL_ENABLE        (3 << 0)
#define CTL_DISABLE       (0 << 0)

// Indicates the maximum number of interrupts that the GIC supports. If ITLinesNumber=N, the
// maximum number of interrupts is 32(N+1). The interrupt ID range is from 0 to (number of IDs – 1). For
// example:
// 0b00011 Up to 128 interrupt lines, interrupt IDs 0-127.
#define TYPE_ITLINESNUMBER_GET(x) ( x & 0xF ) 
#define TYPE_IS_SECURE_EXTN_GET(x) (x & (1 << 10))



struct GIC400_distributor{
    reg32 ctl;
    reg32 type;
    reg32 iid;
    reg32 _res0[29];

    // configure interrupt to group 0 (secure) or group 1(non secure)
    reg32 igroup[32];

    //**
    // The GICD_ISENABLERs provide a Set-enable bit for each interrupt supported by the GIC.
    // Writing 1 to a Set-enable bit enables forwarding of the corresponding interrupt from the
    // Distributor to the CPU interfaces. Reading a bit identifies whether the interrupt is enabled.
    // isenable[0] банкуются для каждого ядра
    //**
    reg32 isenable[32];
    reg32 icenable[32];

    // • set the pending state by writing to the appropriate GICD_ISPENDRn bit
    // • clear the pending state by writing to the appropriate GICD_ICPENDRn bit.
    // pend[0] банкуются для каждого ядра
    reg32 ispend[32];
    reg32 icpend[32];

    // active[0] банкуются для каждого ядра
    reg32 isactive[32];
    reg32 icactive[32];

    // IPRIORITY, N = (8 * (ITLinesNumber + 1))
    //In a multiprocessor implementation, GICD_IPRIORITYR0 to GICD_IPRIORITYR7 are
    //banked for each connected processor. These registers hold the Priority fields for interrupts 0-31.
    reg32 ipriority[256];
    reg32 istargets[256];
    //ICFG, N = (2 * (ITLinesNumber + 1))
    //bit 1-> for SPI programmable, for PPI implementation defined, for SGI not programmable
    //bit 1-> 0 level-sensetive, 1 edge-triggered
    //bit 0-> only where handling mode of peripheral interrupts is configurable
    //bit 0-> 0 interrupt handled using N-N model
    //bit 0-> 1 interrupt handled using 1-N model
    reg32 icfg[62];
    reg32 ppis;
    reg32 spis[127];
    reg32 sgi;
    reg32 _res1[3];
    reg32 cpendsgi[4];
    reg32 spendsgi[4];
    reg32 _res2[40];
    reg32 pid[8];
    reg32 cid[4];
} ;


// — GICC_IAR, GICC_EOIR, and GICC_HPPIR for Group 0 interrupts
// — GICC_AIAR, GICC_AEOIR, and GICC_AHPPIR for Group 1 interrupts
struct GIC400_CPU_interfaces{
//end of interrupt (EOI) behavior.
    reg32 ctl;

    // interrupt priority threshold. If interrupt priority gith then PM, then interrupt core
    reg32 pm;
    reg32 bp;
    // ask register. Return Interrupt ID, for SGI source processor ID.
    //  interrupt ID 1023, indicating a spurious interrupt
    reg32 ia;
    reg32 eoi;
    reg32 rp;
    reg32 hppi;
    reg32 abp;
    reg32 aia;
    reg32 aeoi;
    reg32 ahppi;
    reg32 _res0[41];
    reg32 ap;
    reg32 _res1[3];
    reg32 nasp;
    reg32 _res2[6];
    reg32 iid;
    reg32 _res3[960];
    reg32 dir;
};


#define GIC400_DISTRIBUTOR ((struct GIC400_distributor *) (GIC400_BASE + 0x1000))
#define GIC400_INTERFACES ((struct GIC400_CPU_interfaces *) (GIC400_BASE + 0x2000))

// int gic400_init(void* interrupt_controller_base);
void gic400_turn_off_distributor(void);

int gic400_init(void);

void gic400_enable_sys_timer(uint32_t timer_num);
uint32_t get_num_of_it_lines_support(void);
#endif