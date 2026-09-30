
/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#include <drivers/mmc.h>

#include <linux/kernel.h>

#include <printf.h>
#include <bootmode.h>
#include <image_format.h>
#include <fel.h>

image_header_t image_header = {};
uint64_t boot_addr = 0;

static void load_images_mmc(void) {
    printf("Loading image header\n");
    if (mmc_read_blocks(80, 1, &image_header) != 0) {
        printf("Failed to read image header\n");
        while (1);
    }

	printf("Image header: magic=%08x\n", image_header.magic);
	if (image_header.magic != IMG_MAGIC) {
		printf("Invalid image magic\n");
		while (1);
	}

    if (image_header.cnt > IMG_MAX_ENTRIES) {
        printf("Too many image entries: %u\n", image_header.cnt);
        while (1);
    }

	for (uint32_t i = 0; i < image_header.cnt; i++) {
		image_entry_t *entry = &image_header.entries[i];
		printf("Entry %d: name=%s, load_addr=0x%llx, size=%llu, offset_blocks=%llu\n",
			i, entry->name, entry->load_addr, entry->size, entry->offset_blocks);

		// First image always takes priority.
		if (i == 0)
			boot_addr = entry->load_addr;

		if(mmc_read_blocks(80 + entry->offset_blocks, DIV_ROUND_UP(entry->size, 512), (void *)entry->load_addr) != 0) {
			printf("Failed to read image block\n");
			while (1);
		}
		printf("Loaded image %s to 0x%llx\n", entry->name, entry->load_addr);
	}

	printf("Jumping to image at 0x%llx\n", boot_addr);
	((void (*)(void))boot_addr)();
}

void load_and_boot_images(uint64_t dram_size, int boot_source) {
    struct boot_file_head *egon_head = (void *)0x00020000;

    printf("Updating SPL header...\n");
    egon_head->spl_signature[3] = SPL_DRAM_HEADER_VERSION;
    egon_head->dram_size = dram_size >> 20;

    switch (boot_source) {
        case SUNXI_BOOTED_FROM_MMC0:
            printf("Booted from SD card\n");
            printf("Loading image from SD card...\n");
            load_images_mmc();
            break;
        case SUNXI_INVALID_BOOT_SOURCE:
            printf("Booted from FEL\n");
            printf("Returning to FEL mode, SP: 0x%x, LR: 0x%x\n", fel_stash.sp, fel_stash.lr);
            return_to_fel(fel_stash.sp, fel_stash.lr);
            break;
        default:
            printf("Unsupported boot source: %d\n", boot_source);
            break;
    }
}
