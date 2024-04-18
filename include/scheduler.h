#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include "common.h"




typedef enum {
    RUNABLE,
    RUNNING,
    SLEEP,
} task_state;

struct trapframe{
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;        
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29_fp; // frame pointer
    uint64_t x30_lr; // link register

    uint64_t sp_el0;
    uint64_t elr_el1;
    uint64_t spsr_el1;

    uint64_t esr_el1;
    uint64_t far_el1;
};

struct task{
    struct trapframe * trapframe;
    uint64_t ttbr0_el1;
    task_state state;
    uint64_t preempt_count;
    bool pure;
};

struct cpu{
    struct task current_task;
};

extern struct cpu cpus[];

void schedule(void);
struct cpu * my_cpu(void);
void switch_to(struct task * new_task);

#define EL0t_INTR_ON (0x0 | (~0xf << 6))
#define EL1h_INTR_ON (0x5 | (~0xf << 6))

struct task create_task(uint8_t (*main)(void), uint64_t spsr_el1);
void release_restore_return(void);
uint8_t fork(uint8_t (*main)(void));

#endif