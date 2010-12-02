ENTRY(__embios_executable_hdr)
OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)

MEMORY
{
    RAM : ORIGIN = 0x08000000, LENGTH = 0x01f00000
}

SECTIONS
{
    .bss (NOLOAD) :
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
        *(.stack*)
    } > RAM

    .text 0x08f00000:
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
        . = ALIGN(0x10);
	_scriptstart = .;
    } > RAM

    /DISCARD/ :
    {
        *(.eh_frame)
    }

}
