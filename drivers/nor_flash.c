/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#include <printf.h>

#include <drivers/timer.h>
#include <drivers/spi.h>
#include <drivers/nor_flash.h>

uint64_t nor_flash_size = 0;

static void spi_nor_read_chunk(uint32_t addr, uint8_t *buf, uint32_t len)
{
	uint8_t cmd[4] = {
		NOR_CMD_READ,
		(uint8_t)(addr >> 16),
		(uint8_t)(addr >> 8),
		(uint8_t)addr,
	};

	spi_transfer(cmd, sizeof(cmd), buf, len);

	udelay(1);
}

void spi_nor_read(uint32_t addr, void *buf, uint32_t len)
{
    if (addr + len > nor_flash_size) {
        printf("Error: Attempt to read beyond NOR flash size. Requested: %u, Available: %llu\n", addr + len, nor_flash_size);
        return;
    }

	uint8_t *buf8 = buf;

	while (len > 0) {
		uint32_t chunk = len > NOR_READ_CHUNK_MAX ? NOR_READ_CHUNK_MAX : len;

		spi_nor_read_chunk(addr, buf8, chunk);

		addr += chunk;
		buf8 += chunk;
		len -= chunk;
	}
}

int spi_nor_read_blocks(uint64_t start_block, uint64_t block_count, void *buffer)
{
	spi_nor_read((uint32_t)(start_block * 512), buffer, (uint32_t)(block_count * 512));
	return 0;
}

static char *get_flash_manufacturer(uint8_t manufacturer_id) {
    switch (manufacturer_id) {
        case 0xEF:
            return "Winbond";
        case 0xC2:
            return "Macronix";
        case 0x20:
            return "Micron";
        case 0x1C:
            return "EON";
        case 0x1F:
            return "Atmel";
        case 0x5E:
            return "Zbit";
        default:
            return "Unknown";
    }
}

void spi_nor_init(void) {
    uint8_t id[3];

    uint8_t cmd[4] = {
        NOR_CMD_READ_ID,
        0x00,
        0x00,
        0x00,
    };

    spi_transfer(cmd, sizeof(cmd), id, sizeof(id));
    nor_flash_size = 1 << (id[2] & 0x1F);
    printf("NOR Flash Manufacturer: %s, Device ID: %02X%02X, Size: %llu bytes\n", get_flash_manufacturer(id[0]), id[1], id[2], nor_flash_size);
}
