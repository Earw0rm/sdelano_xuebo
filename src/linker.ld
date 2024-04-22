######################
#https://sourceware.org/binutils/docs/ld/Scripts.html#Scripts
######################

ENTRY(_start)


SECTIONS
{
    _boot_start_addr = .;

    .boot : {
         *(.text.boot)      
         *(.data.boot)
    }

    . += 0x1000;
    . = ALIGN(4096);
    
    _boot_end_addr = .;

    . += 0x5000;
    . = ALIGN(4096);


    _kernel_text_start_addr = .;    

    . += 0x1000;
    . = ALIGN(4096);

    .text :  { 
        *(.text)
        . += 0x1000;
        . = ALIGN(4096);
        _el1vec_start = .;
        *(.el1vec)
        . += 0x1000;
        . = ALIGN(4096);
        _el1vec_end = .;
        . += 0x1000;
        . = ALIGN(4096);
    }
    
    . += 0x1000;
    . = ALIGN(4096);

    _kernel_text_end_addr = .;

    _kernel_data_start_addr = .;

    _thread_shared_begin = .;


    . += 0x1000;
    . = ALIGN(4096);

    .thread_shared : {
        *(.thread_shared)
    }
    . += 0x1000;
    . = ALIGN(4096);

    _thread_shared_end = .;
    
    . += 0x1000;
    . = ALIGN(4096);

    .rodata : {
         *(.rodata)
    }

    . += 0x1000;
    . = ALIGN(4096);


    .data : { 
        *(.data) 
    }
    
    . += 1000;
    . = ALIGN(4096);


    _bss_begin = .;
    
    .bss : { 

        *(.bss*) 

    }

    . += 1000;
    . = ALIGN(4096);    
    
    _bss_end = .;

    . += 0x1000;
    . = ALIGN(4096);




    _end_of_kernel_addr = .;
}