#ifndef _MEMLAYOUT_H
#define _MEMLAYOUT_H

#ifndef __ASSEMBLER__
//linker.ld
extern volatile const char _boot_end_addr;
extern volatile const char _kernel_text_start_addr;

extern volatile const char _kernel_text_end_addr;
extern volatile const char _end_of_kernel_addr;

extern volatile const char _kernel_shared_data_begin;
extern volatile const char _kernel_shared_data_end;

#endif


//###################kernel###########################
#define MAXVA                 0x0000FFFFFFFFFFFF
#define LAYOUT_TOP_GUARD      0x0000fffffffff000ull

#define LAYOUT_MY_CPU_BASE    (0x0000ffffffffb000ull)
#define LAYOUT_MY_CPU(cpu_id) (LAYOUT_MY_CPU_BASE + 0x1000*cpu_id) 
#define MY_CPU_GUARD           0x0000ffffffffa000ull

#define KSTACK(cpu_id) (0x0000ffffffff9000ull - 0x2000*cpu_id)

// legacy master view of physical memory
#define MEM_PASTOP        (0x1f9990000) // 8gb rpi 
#define MEM_SDRAM_BOT     (0x100000000)
#define MEM_SDRAM_TOP     (0xFC000000)

// kernel must start in 0x80000000
#define MEM_KERN_END                 (&_kernel_text_end_addr)
#define MEM_KERNEL_SHARED_DATA_END   (&_kernel_shared_data_begin)
#define MEM_KERNEL_SHARED_DATA_BEGIN (&_kernel_shared_data_begin)
#define MEM_KERN_TEXT_START          (&_kernel_text_start_addr)

#define MEM_VC_BASE_TOP   (0x40000000)
#define MEM_VC_BASE_BOT   (MEM_VC_BASE_TOP - 0x10000000) // 256 mb vc sdram
#define MEM_START         (&_boot_end_addr)


//###################user###########################

#define MEM_USER_MAXVA      (0x80000000ull)
//trampoline it is pointer to exception vectors that live in kernel
//in user process we set vectors equals to kernel vector
#define MEM_USER_TRAMPOLINE (0xffffafffull)
#define MEM_USER_TRAPFRAME  (0xffff1fffull)

#define MEM_USER_STACK      (0x79999000ull)
#define MEM_USER_START      (0x0)


#endif 