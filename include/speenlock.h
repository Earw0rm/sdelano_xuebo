#ifndef _SPEENLOCK_H
#define _SPEENLOCK_H

#include "common.h"
/**
 * https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
*/

// armv8 ;da
// spin_lock:
//     MOV Wt, #1             // Устанавливаем значение, которое мы хотим записать
// lock_retry:
//     LDAXR Ws, [lock]       // Атомарно загружаем текущее значение замка
//     CBNZ Ws, lock_retry    // Если замок уже занят, повторяем попытку
//     STXR Ws, Wt, [lock]    // Пытаемся атомарно установить замок
//     CBNZ Ws, lock_retry    // Если STXR не удался, повторяем все снова
//     RET                    // Замок установлен, возвращаемся из функции

// spin_unlock:
//     MOV Wt, #0             // Сбрасываем замок
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