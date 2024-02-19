#ifndef _ARM_SYSREGS_H
#define _ARM_SYSREGS_H

// ***************************************
// SCTLR_EL1, System Control Register (EL1), Page 6972 of AArch64-Reference-Manual (A-profile architecture reference manual).
// ***************************************
#define SCTLR_RESERVED ((3 << 28) | (3 << 22) | (1 << 20) | (1 << 11))

#define SCTLR_EE_LITTLE_ENDIAN  (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN (0 << 24)
#define SCTLR_I_CACHE_DISABLED  (0 << 12)
#define SCTLR_D_CACHE_DISABLED  (0 << 2)
#define SCTLR_MMU_DISABLED      (0 << 0)
#define SCTLR_MMU_ENABLED       (1 << 0)


#define SCTLR_VALUE_MMU_DISABLED	(SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

// ***************************************
// HCR_EL2, Hypervisor Configuration Register (EL2), Page 6617 of AArch64-Reference-Manual (A-profile architecture reference manual).
// ***************************************

#define HCR_RW    (1 << 31)
#define HCR_VALUE HCR_RW

// ***************************************
// SCR_EL3, Secure Configuration Register (EL3), Page 6947 of AArch64-Reference-Manual (A-profile architecture reference manual).    
// ***************************************

#define SCR_RESERVED (3 << 4)
#define SCR_RW		 (1 << 10)
// broadcom 2711 does not imlement FEAT_RME
// level el0 and el1 are non secure. So, they cannot configure 
// gic400 or another stuff. 
#define SCR_NS		 (1 << 0) 
#define SCR_VALUE	 ( SCR_RESERVED | SCR_RW | SCR_NS)

// ***************************************
// SPSR_EL3, Saved Program Status Register (EL3) Page 819 of AArch64-Reference-Manual (A-profile architecture reference manual).
// Эта конфигурация spsr нужна что бы первый раз перейти из EL3 в EL1    
// младшие 3 бита, это предыдущий EL и режим. 
// самый младший бит это режим(t когда 0 и h когда 1). Это значение младшего бита берется из PSTATE.SP и копируется в него
// обратно при возврате из исключения
// ***************************************

#define SPSR_MASK_ALL (7 << 6) 
#define SPSR_EL1h     (5 << 0) // last time we are EL1 with h mode stack pointer (hypervisor)
#define SPSR_VALUE    (SPSR_MASK_ALL | SPSR_EL1h)


// ***************************************
// ESR_EL1, Exception Syndrome Register (EL1) Page 6438 of AArch64-Reference-Manual (A-profile architecture reference manual).
// Most important => EC, bits [31:26]
// Exception Class. Indicates the reason for the exception that this register holds information about.
// ***************************************
#define ESR_EL1_REASON_SHIFT      26
#define ESR_EL1_REASON_SVC (0b010101U << 26)


#endif 