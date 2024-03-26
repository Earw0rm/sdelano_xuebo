#ifndef _SPEENLOCK_H
#define _SPEENLOCK_H

#include "common.h"
/**
 * https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
 * 
 * 
 * B2.9.2 Exclusive access instructions and Shareable memory locations
*/


struct speenlock{    
    char locked;
    char *name;
    uint8_t cpu_num;
//    struct cpu *cpu; // currenct cpu that holding lock
};


/**
 * __sync_lock_test_and_set(&value, 1)
 * __sync_synchronize()
 * turn_off interruptions
*/
void acquire(struct speenlock *);
/**
 * __sync_synchronize()
 * __sync_lock_release(&value)
 * turn_on interruptions
*/
void release(struct speenlock *);





#endif