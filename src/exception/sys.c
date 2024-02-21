#include "sys.h"
#include "mini_uart.h"
#include "printf.h"

void sys_write(char * buf){
    printf(buf);
}

// int sys_clone(uint64_t stack){

// }

// uint64_t sys_malloc(void){

// }

// void sys_exit(){

// }


void * const syscall_table[] = {
    sys_write,
//    sys_malloc,
//     sys_clone,
//     sys_exit
};

void handle_syscall(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, 
                    uint64_t x5, uint64_t x6, uint64_t x7, uint64_t syscall_num){
    if(syscall_num >= __NR_syscalls){
        //error
        return; 
    }
    void* res = syscall_table[syscall_num];
    // call_table_and_ret(x0, x1, x2, x3, x4, x5, x6, x7, (uint64_t) res);
    // error 
}