#ifndef _CFG_EL3
#define _CFG_EL3

#ifndef __ASSEMBLER__

void configure_el3(void);
void kpgdentry_init(uint64_t tbl_adr, uint64_t va);

#endif 
#endif