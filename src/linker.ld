######################
#https://sourceware.org/binutils/docs/ld/Scripts.html#Scripts
######################


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

    .data : { 
        *(.data) 
    }
  
    bss_begin = .;
    

    .bss : { 

        *(.bss*) 

    }

    bss_end = .;
    . += 0x1000;
    _end_of_kernel_addr = .;
}