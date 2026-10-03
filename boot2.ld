ENTRY(stage_two)

SECTIONS
{
    . = 0x00000000;

    .stage_two_entry :
    {
        KEEP(*(.stage_two_entry))
    }

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(.bss*)
        *(COMMON)
    }
}
