######################
#https://sourceware.org/binutils/docs/ld/Scripts.html#Scripts
######################

ENTRY(_start)

MEMORY
{
  RAM (rwx) : ORIGIN = 0x0000000000000000, LENGTH = 1024*8M
}

SECTIONS
{
    _boot_start_addr = .;
    .text.boot : {
    
         *(.text.boot)      
    } > RAM

    .text :  { 
        *(.text)
    } > RAM

    .rodata : {
         *(.rodata)
    } > RAM

    . += 0x1000;
    . = ALIGN(4096);

    _thread_shared_data_begin = .;

    .data.thread_shared : {
        *(.data.thread_shared)
    } > RAM

    . += 0x1000;
    . = ALIGN(4096);

    _thread_shared_data_end = .;

    . += 0x1000;
    . = ALIGN(4096);
    
    .data : { 
        *(.data) 
    } > RAM
    
    . += 1000;
    . = ALIGN(4096);
    _bss_begin = .;
    

    .bss : { 

        *(.bss*) 

    } > RAM

    . += 1000;
    . = ALIGN(4096);    
    _bss_end = .;

    . += 0x1000;
    . = ALIGN(4096);

    _end_of_kernel_addr = .;
}