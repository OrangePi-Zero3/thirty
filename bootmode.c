/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>
#include <string.h>

#include <bootmode.h>
#include <memory.h>

int sunxi_get_boot_source(void)
{
	struct boot_file_head *egon_head = (void *)0x00020000;

	if (!memcmp(egon_head->magic, BOOT0_MAGIC, 8))
		return readb(&egon_head->boot_media);

	/* Not a valid image, so we must have been booted via FEL. */
	return SUNXI_INVALID_BOOT_SOURCE;
}