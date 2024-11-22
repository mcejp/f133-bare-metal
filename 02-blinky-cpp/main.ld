MEMORY {
    FLASH : ORIGIN = 0x40000000, LENGTH = 32M
}

SECTIONS {
    .text : { *(.text*) } > FLASH
    .rodata : { *(.rodata*) } > FLASH
    .bss : { *(.bss*) } > FLASH
}
