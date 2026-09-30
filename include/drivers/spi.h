/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <bitops.h>

#pragma once

#define SPI0_BASE               0x05010000

#define SPI_GCR                 0x04
#define SPI_TCR                 0x08
#define SPI_FIFO_STA            0x1C
#define SPI_MBC                 0x30
#define SPI_MTC                 0x34
#define SPI_BCC                 0x38
#define SPI_TXD                 0x200
#define SPI_RXD                 0x300

#define SPI_GCR_ENABLE          BIT(0)
#define SPI_GCR_MASTER          BIT(1)
#define SPI_GCR_SRST            BIT(31)

#define SPI_TCR_XCH             BIT(31)

#define SPI_FIFO_RXCNT_MASK     0x7F

#define SPI_FIFO_DEPTH          64

void spi_init(void);
void spi_transfer(const uint8_t *tx, uint32_t tx_len, uint8_t *rx, uint32_t rx_len);
