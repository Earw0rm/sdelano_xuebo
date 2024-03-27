######################
#https://sourceware.org/binutils/docs/ld/Scripts.html#Scripts
######################

ENTRY(_start)

SECTIONS
{
    _boot_start_addr = .;
    .text.boot : {
    
         *(.text.boot)      
    }

    .text :  { 
        *(.text)
    }

    .rodata : {
         *(.rodata)
    }

    . += 0x1000;
    . = ALIGN(4096);

    _thread_shared_data_begin = .;

    .data.thread_shared : {
        *(.data.thread_shared)
    }

    . += 0x1000;
    . = ALIGN(4096);
    
    _thread_shared_data_end = .;

    .data : { 
        *(.data) 
    }
    
    . += 1000;
    _bss_begin = .;
    

    .bss : { 

        *(.bss*) 

    }
    
    _bss_end = .;
    . += 0x1000;
    . = ALIGN(4096);
    _end_of_kernel_addr = .;
}