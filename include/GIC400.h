#ifndef _GIC400_H
#define _GIC400_H

#include "common.h"
#include "base.h"

// TODO: 3.2.4 Interrupt handling state machine
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
// 216 -> last interrupt id 
// 0-15 id -> SGI. 16-31 -> PPI. 
//**
// gic_architecture_specification 35 page
// general interrupt flow 
//**

//GICD_CTRLR
#define CTL_ENABLE_GRP_0  (1 << 0)
#define CTL_ENABLE_GRP_1  (2 << 0)
#define CTL_ENABLE        (3 << 0) // ( 3 << 0 )
#define CTL_DISABLE       (0 << 0)
#define GIC_SPURIOUS_INTR 1023
// Indicates the maximum number of interrupts that the GIC supports. If ITLinesNumber=N, the
// maximum number of interrupts is 32(N+1). The interrupt ID range is from 0 to (number of IDs – 1). For
// example:
// 0b00011 Up to 128 interrupt lines, interrupt IDs 0-127.
#define GIC_TYPE_ITLINESNUMBER(x)  (x & 0x1f) 
#define GIC_TYPE_IS_SECURE_EXTN(x) ((x & (1 << 10)) >> 10)
#define GIC_TYPE_CPU_NUMBER(x)     ((x & (0x7 << 5)) >> 5)
#define GIC_TYPE_LSPI(x)           ((x & (0x1f << 11)) >> 11) // 0b11111 mean 31 LOCABLE spi's

struct GIC400_distributor{
    // non-secure bit[0] => enable interrupt forwarder
    reg32 ctl;
     // info about gic configuration
    reg32 type;
    // Provides information about the implementer and revision of the Distributor
    reg32 iid;
    // 11 + 15
    reg32 _res0[29];


    // configure interrupt to group 0 (secure) or group 1(non secure)
    // bit[0] = interrupt 0 in gpoup (0 or 1)
    // bit[1] = interrupt 1 in gpoup (0 or 1)
    // GICD_IGROUPR0 banking
    reg32 igroup[32];


    /** The GICD_ISENABLERs provide a Set-enable bit for each interrupt supported by the GIC.
    * Writing 1 to a Set-enable bit enables forwarding of the corresponding interrupt from the
    * Distributor to the CPU interfaces. Reading a bit identifies whether the interrupt is enabled.
    * 
    * isenable[0] is banking, This register holds the set-enable bits for 
    * interrupts 0-31 
    * The number of implemented GICD_ISENABLERs is (GICD_TYPER.ITLinesNumber+1)
    */
    reg32 isenable[32];
    // same logic as isenable
    reg32 icenable[32];

    // set the pending state by writing to the appropriate GICD_ISPENDRn bit
    // clear the pending state by writing to the appropriate GICD_ICPENDRn bit.
    // ispend[0] & icpend[0] is banking
    reg32 ispend[32];
    reg32 icpend[32];

    // Activates the corresponding interrupt, if it is not already active.
    // isactive[0] & icactive[0] is banking
    reg32 isactive[32];
    reg32 icactive[32];

    // IPRIORITY, N = (8 * (ITLinesNumber + 1))
    // ipriority0 to ipriority7 is banked
    // These registers hold the Priority fields for interrupts 0-31.
    // priority intr 0 => [0:7], 
    // priority intr 1 => [8:15], 
    // priority intr 2 => [16:23], 
    // priority intr 3 => [24:31].
    reg32 ipriority[256];
    // same logic as ipriority but 0-7 are read only registers
    // target CPU for each interrupt
    reg32 istargets[256];

    // The GICD_ICFGRs provide a 2-bit Int_config field for each interrupt supported by the GIC.
    // This field identifies whether the corresponding interrupt is edge-triggered or level-sensitive, 
    // ICFG, N = (2 * (ITLinesNumber + 1))
    // For SGIs, Int_config fields are read-only, meaning that GICD_ICFGR0 is read-only
    // bit 1-> for SPI programmable, for PPI implementation defined, for SGI not programmable
    // bit 1-> 0 level-sensetive, 1 edge-triggered
    // bit 0-> only where handling mode of peripheral interrupts is configurable
    // bit 0-> 0 interrupt handled using N-N model
    // bit 0-> 1 interrupt handled using 1-N model

    //     In a multiprocessor implementation, if bit[1] of the Int_config field for any PPI is
    // programmable then GICD_ICFGR1 is banked for each connected processor. This register
    // holds the Int_config fields for the PPIs, interrupts 16-31.
    reg32 icfg[62];


    // next registers in distributor are implementation defined.
    // see gic400 4-73 
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


//CPU interface is banging for each CPU core 
// — GICC_IAR(interrupt ask), GICC_EOIR(end of interrupt), and GICC_HPPIR for Group 0 interrupts
// — GICC_AIAR, GICC_AEOIR, and GICC_AHPPIR for Group 1 interrupts
struct GIC400_CPU_interfaces{
    //end of interrupt (EOI) behavior. 4-124 of gic arch ref manual
    // • Accesses from Secure state. This control applies to the handling of both Group 0 and
    // Group 1 interrupts.
    // • Accesses from Non-secure state. This control only applies to the handling of Group 1
    // interrupts.
    reg32 ctl;


    // interrupt priority threshold. If interrupt priority gith then PM, then interrupt core
    reg32 pm;
    reg32 bp;
    
    // The processor reads this register to obtain the interrupt ID of the signaled interrupt. This
    // read acts as an acknowledge for the interrupt.
    reg32 ia;
    //     A processor writes to this register to inform the CPU interface either:
    // • that it has completed the processing of the specified interrupt
    // • in a GICv2 implementation, when the appropriate GICC_CTLR.EOImode bit is set
    // to 1, to indicate that the interface should perform priority drop for the specified
    // interrupt.

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

typedef enum{
    SYS_TIMER_0 = 96,
    SYS_TIMER_1 = 97,
    SYS_TIMER_2 = 98,
    SYS_TIMER_3 = 99,
} INTR_ID;

#define GIC400_DISTRIBUTOR ((struct GIC400_distributor *) (GIC400_BASE + 0x1000))
#define GIC400_INTERFACES ((struct GIC400_CPU_interfaces *) (GIC400_BASE + 0x2000))

void gic400_global_init(void);
void gic400_local_init(void);

void gic400_turn_offd(void);
void gic400_turn_ond(void);
void gic400_turn_offi(void);
void gic400_turn_oni(void);

void gic400_enable_sys_timer(uint32_t timer_num);
uint32_t get_gic400_info(void);




#endif