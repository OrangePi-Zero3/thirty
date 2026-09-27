/*
 * sun50i H616 LPDDR4-2133 timings, as programmed by Allwinner's boot0
 * for orangepi zero3 with the H618 and LPDDR4 memory.
 *
 * (C) Copyright 2023 Mikhail Kalashnikov <iuncuim@gmail.com>
 *   Based on H6 DDR3 timings:
 *   (C) Copyright 2020 Jernej Skrabec <jernej.skrabec@siol.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <stdint.h>

#include <memory.h>

#include <drivers/dram.h>

void mctl_set_timing_params(const struct dram_para *para)
{
	struct sunxi_mctl_ctl_reg * const mctl_ctl =
			(struct sunxi_mctl_ctl_reg *)SUNXI_DRAM_CTL0_BASE;

	uint8_t tccd		= 4;
	uint8_t tfaw		= ns_to_t(40);
	uint8_t trrd		= max(ns_to_t(10), 2);
	uint8_t trcd		= max(ns_to_t(18), 2);
	uint8_t trc		= ns_to_t(65);
	uint8_t txp		= max(ns_to_t(8), 2);
	uint8_t trtp		= 4;
	uint8_t trp		= ns_to_t(21);
	uint8_t tras		= ns_to_t(42);
	uint16_t trefi	= ns_to_t(3904) / 32;
	uint16_t trfc	= ns_to_t(280);
	uint16_t txsr	= ns_to_t(190);

	uint8_t tmrw		= max(ns_to_t(14), 5);
	uint8_t tmrd		= tmrw;
	uint8_t tmod		= 12;
	uint8_t tcke		= max(ns_to_t(15), 2);
	uint8_t tcksrx	= max(ns_to_t(2), 2);
	uint8_t tcksre	= max(ns_to_t(5), 2);
	uint8_t tckesr	= tcke;
	uint8_t trasmax	= (trefi * 9) / 32;
	uint8_t txs		= 4;
	uint8_t txsdll	= 16;
	uint8_t txsabort	= 4;
	uint8_t txsfast	= 4;
	uint8_t tcl		= 10;
	uint8_t tcwl		= 5;
	uint8_t t_rdata_en	= 17;
	uint8_t tphy_wrlat	= 5;

	uint8_t twtp		= 24;
	uint8_t twr2rd	= max(trrd, (uint8_t)4) + 14;
	uint8_t trd2wr	= (ns_to_t(4) + 17) - ns_to_t(1);

	/* set DRAM timing */
	writel((twtp << 24) | (tfaw << 16) | (trasmax << 8) | tras,
	       &mctl_ctl->dramtmg[0]);
	writel((txp << 16) | (trtp << 8) | trc, &mctl_ctl->dramtmg[1]);
	writel((tcwl << 24) | (tcl << 16) | (trd2wr << 8) | twr2rd,
	       &mctl_ctl->dramtmg[2]);
	writel((tmrw << 20) | (tmrd << 12) | tmod, &mctl_ctl->dramtmg[3]);
	writel((trcd << 24) | (tccd << 16) | (trrd << 8) | trp,
	       &mctl_ctl->dramtmg[4]);
	writel((tcksrx << 24) | (tcksre << 16) | (tckesr << 8) | tcke,
	       &mctl_ctl->dramtmg[5]);
	/* Value suggested by ZynqMP manual and used by libdram */
	writel((txp + 2) | 0x02020000, &mctl_ctl->dramtmg[6]);
	writel((txsfast << 24) | (txsabort << 16) | (txsdll << 8) | txs,
	       &mctl_ctl->dramtmg[8]);
	writel(0x00020208, &mctl_ctl->dramtmg[9]);
	writel(0xE0C05, &mctl_ctl->dramtmg[10]);
	writel(0x440C021C, &mctl_ctl->dramtmg[11]);
	writel(8, &mctl_ctl->dramtmg[12]);
	writel(0xA100002, &mctl_ctl->dramtmg[13]);
	writel(txsr, &mctl_ctl->dramtmg[14]);

	clrsetbits_le32(&mctl_ctl->init[0], 0xC0000FFF, 0x3f0);
	writel(0x01f20000, &mctl_ctl->init[1]);
	writel(0x00000d05, &mctl_ctl->init[2]);
	writel(0, &mctl_ctl->dfimisc);
	writel(0x0034001b, &mctl_ctl->init[3]);
	writel(0x00330000, &mctl_ctl->init[4]);
	writel(0x00040072, &mctl_ctl->init[6]);
	writel(0x00240009, &mctl_ctl->init[7]);

	clrsetbits_le32(&mctl_ctl->rankctl, 0xff0, 0x660);

	/* Configure DFI timing */
	writel(tphy_wrlat | 0x2000000 | (t_rdata_en << 16) | 0x808000,
	       &mctl_ctl->dfitmg0);
	writel(0x100202, &mctl_ctl->dfitmg1);

	/* set refresh timing */
	writel((trefi << 16) | trfc, &mctl_ctl->rfshtmg);
}
