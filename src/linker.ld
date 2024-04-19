######################
#https://sourceware.org/binutils/docs/ld/Scripts.html#Scripts
######################

ENTRY(_start)

MEMORY
{
  BOOT     (rwx)  : ORIGIN = 0x0,               LENGTH = 4096*10B,
  RAM      (rwx) : ORIGIN = 0x0,                LENGTH = 2048M
}

SECTIONS
{
    _boot_start_addr = .;
    .text.boot : {
         *(.text.boot)      
    } > RAM

    .data.boot : {
         *(.data.boot)      
    } > RAM
    
    . += 0x1000;
    . = ALIGN(4096);
    
    _boot_end_addr = .;

    . += 0x5000;
    . = ALIGN(4096);

    _kernel_text_start_addr = .;
    .text :  { 
        *(.text)
    } > RAM
    
    . += 0x1000;
    . = ALIGN(4096);
    _kernel_text_end_addr = .;

    _kernel_data_start_addr = .;
    _kernel_shared_data_begin = .;

    .data.thread_shared : {
        *(.data.thread_shared)
    } > RAM

    . += 0x1000;
    . = ALIGN(4096);

    _kernel_shared_data_end = .;



    .rodata : {
         *(.rodata)
    } > RAM

    . += 0x1000;
    . = ALIGN(4096);
    
    _el1vec_start = .;
    
    . += 0x1000;
    . = ALIGN(4096);

    .el1vec : {
        *(.el1vec)
    } > RAM

    . += 0x1000;
    . = ALIGN(4096);

    _el1vec_end = .;

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