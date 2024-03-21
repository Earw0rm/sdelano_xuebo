#ifndef _SPEENLOCK_H
#define _SPEENLOCK_H

#include "common.h"


//ldrex strex 
struct speenlock{
    bool locked;    
    char *name;
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