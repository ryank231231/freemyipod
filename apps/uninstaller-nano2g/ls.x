ENTRY(__embios_executable_hdr)
OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)

MEMORY
{
    RAM : ORIGIN = 0x08800000, LENGTH = 0x01600000
}

SECTIONS
{
    .text :
    {
	KEEP(.execheader*)
	*(.execheader*)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        . = ALIGN(0x4);
        *(.rodata*)
        . = ALIGN(0x4);
        *(.data*)
        . = ALIGN(0x4);
    } > RAM

    .bss (NOLOAD) :
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
        *(.stack*)
    } > RAM

    /DISCARD/ :
    {
        *(.eh_frame)
    }

}
