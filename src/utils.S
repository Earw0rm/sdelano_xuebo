





//extern void put32(uint64_t, uint32_t);
.globl put32
put32:
    str w1, [x0]
    ret

//extern uint32_t get32(uint64_t);
.globl get32
get32:
    ldr w0, [x0]
    ret


.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret
