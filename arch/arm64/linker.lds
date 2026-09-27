/* SPDX-License-Identifier: GPL-2.0+ */
ENTRY(_start)

SECTIONS
{
	. = 0x00020060;

	.start : { *(.start) }
	.text  : { *(.text*) }
	.rodata : { *(.rodata*) }
	.data  : { *(.data*) }

	. = ALIGN(8);
	.bss : {
		__bss_start = .;
		*(.bss*)
		*(COMMON)
		__bss_end = .;
	}

	/DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) *(.ARM.*) }
}
