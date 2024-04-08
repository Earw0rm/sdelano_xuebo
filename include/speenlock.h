#ifndef _SPEENLOCK_H
#define _SPEENLOCK_H

#include "common.h"
/**
 * https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
 * 
 * 
 * B2.9.2 Exclusive access instructions and Shareable memory locations
 * 
 * Because implementations can choose which memory types are treated as Non-cacheable, the only memory types for which it is architecturally guaranteed that a global Exclusives monitor is implemented are:
 * 1) Inner Shareable, Inner Write-Back, Outer Write-Back Normal memory with Read allocation hints and Write allocation hints and not transient. 
 * 2) Outer Shareable, Inner Write-Back, Outer Write-Back Normal memory with Read allocation hints and Write allocation hints and not transient.
 * 
 * 
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