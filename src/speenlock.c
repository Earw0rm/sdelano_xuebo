#include "speenlock.h"
#include "arm/arm_util.h"
/**
 * https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
 * 
*/
void acquire(struct speenlock * lock){
    
    disable_irq();
    

    while(__atomic_test_and_set(&(lock->locked), __ATOMIC_ACQUIRE) != 0){
        asm volatile("nop");
    }

    // lock->cpu_num = get_processor_id();
    // lock->name = "trying_lock";
}



/**
 * __sync_synchronize()
 * __sync_lock_release(&value)
 * turn_on interruptions
*/
void release(struct speenlock *lock){
    lock->cpu_num = -1;
    
    __atomic_clear(&(lock->locked), __ATOMIC_RELEASE);
    
    enable_irq();
}