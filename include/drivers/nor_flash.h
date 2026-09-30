/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#include <drivers/spi.h>

#pragma once

#define NOR_READ_CHUNK_MAX      (SPI_FIFO_DEPTH - 4)

#define NOR_CMD_READ            0x03
#define NOR_CMD_READ_ID         0x9F

void spi_nor_read(uint32_t addr, void *buf, uint32_t len);
int spi_nor_read_blocks(uint64_t start_block, uint64_t block_count, void *buffer);
void spi_nor_init(void);