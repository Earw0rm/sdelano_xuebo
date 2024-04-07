#include "speenlock.h"
#include "arm/arm_util.h"
/**
 * https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
 * 
 * 
 * ldra - load acquire 
 * stlr - store release 
 * 
*/


void acquire(struct speenlock * lock){
    
    disable_irq();
    

    // while( != 0){
    //     asm volatile("nop");
    // }
    __atomic_test_and_set(&(lock->locked), __ATOMIC_ACQ_REL);
    lock->cpu_num = get_processor_id();
    lock->name = "trying_lock";
}



/**
 * __sync_synchronize()
 * __sync_lock_release(&value)
 * turn_on interruptions
*/
void release(struct speenlock *lock){
    lock->cpu_num = -1;
    
    __atomic_clear(&(lock->locked), __ATOMIC_ACQ_REL);
    
    enable_irq();
}