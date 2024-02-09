#include "sys.h"

// by convention, registers x0-x7 are used for syscall arguments
// x8 is used to store syscall number
// NOTE:
// Such wrapper functions are usually not included in the kernel itself 
// — you are more likely to find them in the different language's standard libraries,
// such as (glibc) [https://www.gnu.org/software/libc/]

.globl call_sys_write
call_sys_write:
    mov w8, #SYS_WRITE_N
    svc #0
    ret

.globl call_sys_malloc
call_sys_malloc:
    mov w8, #SYS_MALLOC_N
    svc #0
    ret

.globl call_sys_exit
call_sys_exit:
    mov w8, #SYS_EXIT_N
    svc #0
    ret

