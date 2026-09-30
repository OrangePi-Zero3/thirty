/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#include <drivers/spi.h>
#include <drivers/gpio.h>
#include <drivers/timer.h>

#include <printf.h>

#include <memory.h>

static const int spi0_pins[4] = {
	SUNXI_GPC(0), SUNXI_GPC(2), SUNXI_GPC(3), SUNXI_GPC(4),
};

void spi_init(void)
{
	for (int i = 0; i < 4; i++)
		gpio_configure_pin(spi0_pins[i], MUX_4);

	setbits_le32(SPI0_BASE + SPI_GCR, SPI_GCR_MASTER | SPI_GCR_ENABLE | SPI_GCR_SRST);
	while (readl(SPI0_BASE + SPI_GCR) & SPI_GCR_SRST)
		;

    printf("SPI initialized\n");
}

void spi_transfer(const uint8_t *tx, uint32_t tx_len, uint8_t *rx, uint32_t rx_len)
{
	writel(tx_len + rx_len, SPI0_BASE + SPI_MBC);
	writel(tx_len, SPI0_BASE + SPI_MTC);
	writel(tx_len, SPI0_BASE + SPI_BCC);

	for (uint32_t i = 0; i < tx_len; i++)
		writeb(tx[i], SPI0_BASE + SPI_TXD);

	setbits_le32(SPI0_BASE + SPI_TCR, SPI_TCR_XCH);

	while ((readl(SPI0_BASE + SPI_FIFO_STA) & SPI_FIFO_RXCNT_MASK) < tx_len + rx_len)
		;

	for (uint32_t i = 0; i < tx_len; i++)
		readb(SPI0_BASE + SPI_RXD);

	for (uint32_t i = 0; i < rx_len; i++)
		rx[i] = readb(SPI0_BASE + SPI_RXD);
}