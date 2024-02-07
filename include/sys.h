#ifndef _SYS_H
#define _SYS_H 

#define __NR_syscalls 4

#define SYS_WRITE_N   0
#define SYS_MALLOC_N  1
#define SYS_CLONE_N   2
#define SYS_EXIT_N    3

#ifndef __ASSEMBLER__ 

#include "common.h"

void sys_write(char * buf);
// int sys_fork(void);


void call_sys_write(char * buf);
//int call_sys_clone(uint64_t fn, uint64_t arg, uint64_t stack);
// uint64_t call_sys_malloc(void);
// void call_sys_exit(void);


void handle_syscall(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, 
                    uint64_t x5, uint64_t x6, uint64_t x7, uint64_t syscall_num);
#endif 

#endif 