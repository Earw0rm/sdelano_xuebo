######################
#https://sourceware.org/binutils/docs/ld/Scripts.html#Scripts
######################


SECTIONS
{
    . = 0xffff000000000000;
    _boot_start_addr = .;
    .text.boot : { *(.text.boot) }
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) } 
    bss_end = .;
    . = ALIGN(4096);
    _end_of_kernel_addr = .;
}