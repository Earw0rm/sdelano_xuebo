#ifndef _MEMLAYOUT_H
#define _MEMLAYOUT_H

#ifndef __ASSEMBLER__
//linker.ld
extern volatile const char _boot_end_addr;
extern volatile const char _kernel_text_start_addr;

extern volatile const char _kernel_text_end_addr;
extern volatile const char _end_of_kernel_addr;



extern volatile const char _el1vec_start;
extern volatile const char _el1vec_end;

extern volatile const char _boot_start_addr;

extern volatile const char _thread_shared_begin;
extern volatile const char _thread_shared_end;
#endif


////////////////////////////////////////////////////
//###################kernel########################
////////////////////////////////////////////////////
#define MAXVA                 0x0000FFFFFFFFFFFF
#define LAYOUT_TOP_GUARD      0x0000fffffffff000ull

#define LAYOUT_MY_CPU_BASE    (0x0000ffffffffb000ull)
#define LAYOUT_MY_CPU(cpu_id) (LAYOUT_MY_CPU_BASE + 0x1000*cpu_id) 
#define MY_CPU_GUARD           0x0000ffffffffa000ull

#define KSTACK(cpu_id) (0x0000ffffffff0000ull + 0x2000*cpu_id)

// legacy master view of physical memory
#define MEM_PASTOP        (0x1f9990000) // 8gb rpi 
#define MEM_SDRAM_BOT     (0x100000000)
#define MEM_SDRAM_TOP     (0xFC000000)

// kernel must start in 0x80000000

#define MEM_KERN_END                 ((uint64_t)&_end_of_kernel_addr)

#define MEM_EL1VEC_END               ((uint64_t)&_el1vec_end)
#define MEM_EL1VEC_START             ((uint64_t)&_el1vec_start)
#define MEM_KERNEL_SHARED_DATA_END   ((uint64_t)&_thread_shared_end)
#define MEM_KERNEL_SHARED_DATA_BEGIN ((uint64_t)&_thread_shared_begin)
#define MEM_KERN_TEXT_END            ((uint64_t)&_kernel_text_end_addr)
#define MEM_KERN_TEXT_START          ((uint64_t)&_kernel_text_start_addr)

#define MEM_KERN_START               (0x0)

#define MEM_VC_BASE_TOP   (0x40000000)
#define MEM_VC_BASE_BOT   (MEM_VC_BASE_TOP - 0x10000000) // 256 mb vc sdram
#define MEM_START         (&_boot_end_addr)


#define MEM_TMP_END   (MEM_VC_BASE_BOT)
#define MEM_TMP_START (MEM_KERN_END)

////////////////////////////////////////////////////
//###################user###########################
////////////////////////////////////////////////////

//trampoline it is pointer to exception vectors that live in kernel
//in user process we set vectors equals to kernel vector

//All addresses above 0x80000000 in the user should be accessible only from el1

#define MEM_USER_TRAMPOLINE_END   (MEM_EL1VEC_END)
#define MEM_USER_TRAMPOLINE_START (MEM_EL1VEC_START)

#define MEM_USER_TRAPFRAME  (MEM_USER_TRAMPOLINE_END + 0x1000)


#define MEM_USER_MAXVA      (0x80000000ull)
#define MEM_USER_STACK      (0x79999000ull)
#define MEM_USER_START      (0x0)


#endif 