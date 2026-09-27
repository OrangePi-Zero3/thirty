// SPDX-License-Identifier: GPL-2.0+
/*
 * sun50i H616 platform dram controller driver
 *
 * While controller is very similar to that in H6, PHY is completely
 * unknown. That's why this driver has plenty of magic numbers. Some
 * meaning was nevertheless deduced from strings found in boot0 and
 * known meaning of some dram parameters.
 * This driver supports DDR3, LPDDR3 and LPDDR4 memory. There is no
 * DDR4 support yet.
 *
 * (C) Copyright 2020 Jernej Skrabec <jernej.skrabec@siol.net>
 * (C) Copyright 2023 Mikhail Kalashnikov <iuncuim@gmail.com>
 * (C) Copyright 2026 Umer Uddin <umer.uddin@mentallysanemainliners.org>
 *
 */
#include <stdint.h>

#include <linux/kernel.h>

#include <generated/autoconf.h>

#pragma once

#define SUNXI_DRAM_COM_BASE		0x047FA000
#define SUNXI_DRAM_CTL0_BASE	0x047FB000
#define SUNXI_DRAM_PHY0_BASE	0x04800000
#define SUNXI_CCM_BASE			0x03001000
#define SUNXI_PRCM_BASE			0x07010000

#define CCU_H6_PLL5_CFG			0x010

#define CCU_H6_APB1_CFG			0x520
#define CCU_H6_APB2_CFG			0x524
#define CCU_H6_MBUS_CFG			0x540
#define CCU_H6_DRAM_CLK_CFG		0x800
#define CCU_H6_MBUS_GATE		0x804
#define CCU_H6_DRAM_GATE_RESET	0x80c
#define CCU_PRCM_RES_CAL_CTRL	0x310
#define CCU_PRCM_OHMS240		0x318

/* MBUS clock bit field */
#define MBUS_ENABLE				BIT(31)
#define MBUS_RESET				BIT(30)

#define RESET_SHIFT				(16)
#define GATE_SHIFT				(0)

/* PLL bit fields */
#define CCM_PLL_CTRL_EN			BIT(31)
#define CCM_PLL_LOCK_EN			BIT(29)
#define CCM_PLL_LOCK			BIT(28)
#define CCM_PLL_OUT_EN			BIT(27)

/* DRAM clock bit field */
#define DRAM_MOD_RESET			BIT(30)
#define DRAM_CLK_SRC_PLL5		(0 << 24)

/* pll5 bit field */
#define CCM_PLL5_CTRL_N(n)		(((n) - 1) << 8)

/* master types */
#define MSTR_DEVICETYPE_DDR3	BIT(0)
#define MSTR_DEVICETYPE_LPDDR3	BIT(3)
#define MSTR_DEVICETYPE_LPDDR4	BIT(5)
#define MSTR_2TMODE				BIT(10)
#define MSTR_BUSWIDTH_FULL		(0 << 12)
#define MSTR_BUSWIDTH_HALF		(1 << 12)
#define MSTR_ACTIVE_RANKS(x)	(((x == 2) ? 3 : 1) << 24)
#define MSTR_BURST_LENGTH(x)	(((x) >> 1) << 16)

/* tpr10 bit field */
#define TPR10_CA_BIT_DELAY		BIT(16)
#define TPR10_DX_BIT_DELAY0		BIT(17)
#define TPR10_DX_BIT_DELAY1		BIT(18)
#define TPR10_WRITE_LEVELING	BIT(20)
#define TPR10_READ_CALIBRATION	BIT(21)
#define TPR10_READ_TRAINING		BIT(22)
#define TPR10_WRITE_TRAINING	BIT(23)

/* MBUS configuration*/
#define MBUS_CONF(port, priority, qos, acs, bwl0, bwl1, bwl2)	\
	mbus_configure_port(port, true, priority, \
			    MBUS_QOS_ ## qos, 0, acs, bwl0, bwl1, bwl2)

#ifdef CONFIG_DRAM_SUNXI_PHY_ADDR_MAP_1
static const uint8_t phy_init[] = {
#ifdef CONFIG_SUNXI_DRAM_H616_DDR3_1333
	0x08, 0x02, 0x12, 0x05, 0x15, 0x17, 0x18, 0x0b,
	0x14, 0x07, 0x04, 0x13, 0x0c, 0x00, 0x16, 0x1a,
	0x0a, 0x11, 0x03, 0x10, 0x0e, 0x01, 0x0d, 0x19,
	0x06, 0x09, 0x0f
#elif defined(CONFIG_SUNXI_DRAM_H616_LPDDR3)
	0x18, 0x00, 0x04, 0x09, 0x06, 0x05, 0x02, 0x19,
	0x17, 0x03, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x07,
	0x08, 0x01, 0x1a
#elif defined(CONFIG_SUNXI_DRAM_H616_LPDDR4)
	0x03, 0x00, 0x17, 0x05, 0x02, 0x19, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x01,
	0x18, 0x04, 0x1a
#endif
};
#else /* CONFIG_DRAM_SUNXI_PHY_ADDR_MAP_0 */
static const uint8_t phy_init[] = {
#ifdef CONFIG_SUNXI_DRAM_H616_DDR3_1333
	0x07, 0x0b, 0x02, 0x16, 0x0d, 0x0e, 0x14, 0x19,
	0x0a, 0x15, 0x03, 0x13, 0x04, 0x0c, 0x10, 0x06,
	0x0f, 0x11, 0x1a, 0x01, 0x12, 0x17, 0x00, 0x08,
	0x09, 0x05, 0x18
#elif defined(CONFIG_SUNXI_DRAM_H616_LPDDR3)
	0x18, 0x06, 0x00, 0x05, 0x04, 0x03, 0x09, 0x02,
	0x08, 0x01, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x07,
	0x17, 0x19, 0x1a
#elif defined(CONFIG_SUNXI_DRAM_H616_LPDDR4)
	0x02, 0x00, 0x17, 0x05, 0x04, 0x19, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x01,
	0x18, 0x03, 0x1a
#endif
};
#endif /* CONFIG_DRAM_SUNXI_PHY_ADDR_MAP_0 */
#define MASK_BYTE(reg, nr) (((reg) >> ((nr) * 8)) & 0x1f)

enum sunxi_dram_type {
	SUNXI_DRAM_TYPE_DDR3 = 3,
	SUNXI_DRAM_TYPE_DDR4,
	SUNXI_DRAM_TYPE_LPDDR3 = 7,
	SUNXI_DRAM_TYPE_LPDDR4
};

enum {
	MBUS_QOS_LOWEST = 0,
	MBUS_QOS_LOW,
	MBUS_QOS_HIGH,
	MBUS_QOS_HIGHEST
};

/* MBUS part is largely the same as in H6, except for one special register */
struct sunxi_mctl_com_reg {
	uint32_t cr;			/* 0x000 control register */
	uint8_t reserved_0x004[4];	/* 0x004 */
	uint32_t unk_0x008;		/* 0x008 */
	uint32_t tmr;		/* 0x00c timer register */
	uint8_t reserved_0x010[4];	/* 0x010 */
	uint32_t unk_0x014;		/* 0x014 */
	uint8_t reserved_0x018[8];	/* 0x018 */
	uint32_t maer0;		/* 0x020 master enable register 0 */
	uint32_t maer1;		/* 0x024 master enable register 1 */
	uint32_t maer2;		/* 0x028 master enable register 2 */
	uint8_t reserved_0x02c[468];	/* 0x02c */
	uint32_t bwcr;		/* 0x200 bandwidth control register */
	uint8_t reserved_0x204[12];	/* 0x204 */
	/*
	 * The last master configured by BSP libdram is at 0x49x, so the
	 * size of this struct array is set to 41 (0x29) now.
	 */
	struct {
		uint32_t cfg0;		/* 0x0 */
		uint32_t cfg1;		/* 0x4 */
		uint8_t reserved_0x8[8];	/* 0x8 */
	} master[41];		/* 0x210 + index * 0x10 */
	uint8_t reserved_0x4a0[96];	/* 0x4a0 */
	uint32_t unk_0x500;		/* 0x500 */
};

/*
 * Controller registers seems to be the same or at least very similar
 * to those in H6.
 */
struct sunxi_mctl_ctl_reg {
	uint32_t mstr;		/* 0x000 */
	uint32_t statr;		/* 0x004 unused */
	uint32_t mstr1;		/* 0x008 unused */
	uint32_t clken;		/* 0x00c */
	uint32_t mrctrl0;		/* 0x010 unused */
	uint32_t mrctrl1;		/* 0x014 unused */
	uint32_t mrstatr;		/* 0x018 unused */
	uint32_t mrctrl2;		/* 0x01c unused */
	uint32_t derateen;		/* 0x020 unused */
	uint32_t derateint;		/* 0x024 unused */
	uint8_t reserved_0x028[8];	/* 0x028 */
	uint32_t pwrctl;		/* 0x030 unused */
	uint32_t pwrtmg;		/* 0x034 unused */
	uint32_t hwlpctl;		/* 0x038 unused */
	uint8_t reserved_0x03c[20];	/* 0x03c */
	uint32_t rfshctl0;		/* 0x050 unused */
	uint32_t rfshctl1;		/* 0x054 unused */
	uint8_t reserved_0x058[8];	/* 0x05c */
	uint32_t rfshctl3;		/* 0x060 */
	uint32_t rfshtmg;		/* 0x064 */
	uint8_t reserved_0x068[104];	/* 0x068 */
	uint32_t init[8];		/* 0x0d0 */
	uint32_t dimmctl;		/* 0x0f0 unused */
	uint32_t rankctl;		/* 0x0f4 */
	uint8_t reserved_0x0f8[8];	/* 0x0f8 */
	uint32_t dramtmg[17];	/* 0x100 */
	uint8_t reserved_0x144[60];	/* 0x144 */
	uint32_t zqctl[3];		/* 0x180 */
	uint32_t zqstat;		/* 0x18c unused */
	uint32_t dfitmg0;		/* 0x190 */
	uint32_t dfitmg1;		/* 0x194 */
	uint32_t dfilpcfg[2];	/* 0x198 unused */
	uint32_t dfiupd[3];		/* 0x1a0 */
	uint32_t reserved_0x1ac;	/* 0x1ac */
	uint32_t dfimisc;		/* 0x1b0 */
	uint32_t dfitmg2;		/* 0x1b4 unused */
	uint32_t dfitmg3;		/* 0x1b8 unused */
	uint32_t dfistat;		/* 0x1bc */
	uint32_t dbictl;		/* 0x1c0 */
	uint8_t reserved_0x1c4[60];	/* 0x1c4 */
	uint32_t addrmap[12];	/* 0x200 */
	uint8_t reserved_0x230[16];	/* 0x230 */
	uint32_t odtcfg;		/* 0x240 */
	uint32_t odtmap;		/* 0x244 */
	uint8_t reserved_0x248[8];	/* 0x248 */
	uint32_t sched[2];		/* 0x250 */
	uint8_t reserved_0x258[180];	/* 0x258 */
	uint32_t dbgcmd;		/* 0x30c unused */
	uint32_t dbgstat;		/* 0x310 unused */
	uint8_t reserved_0x314[12];	/* 0x314 */
	uint32_t swctl;		/* 0x320 */
	uint32_t swstat;		/* 0x324 */
	uint8_t reserved_0x328[7768];/* 0x328 */
	uint32_t unk_0x2180;		/* 0x2180 */
	uint8_t reserved_0x2184[188];/* 0x2184 */
	uint32_t unk_0x2240;		/* 0x2240 */
	uint8_t reserved_0x2244[3900];/* 0x2244 */
	uint32_t unk_0x3180;		/* 0x3180 */
	uint8_t reserved_0x3184[188];/* 0x3184 */
	uint32_t unk_0x3240;		/* 0x3240 */
	uint8_t reserved_0x3244[3900];/* 0x3244 */
	uint32_t unk_0x4180;		/* 0x4180 */
	uint8_t reserved_0x4184[188];/* 0x4184 */
	uint32_t unk_0x4240;		/* 0x4240 */
};

struct dram_para {
	uint32_t clk;
	enum sunxi_dram_type type;
	uint32_t dx_odt;
	uint32_t dx_dri;
	uint32_t ca_dri;
	uint32_t odt_en;
	uint32_t tpr0;
	uint32_t tpr2;
	uint32_t tpr6;
	uint32_t tpr10;
	uint32_t tpr11;
	uint32_t tpr12;
};

struct dram_config {
	uint8_t cols;
	uint8_t rows;
	uint8_t ranks;
	uint8_t bus_full_width;
};

static inline int ns_to_t(int nanoseconds)
{
	const unsigned int ctrl_freq = CONFIG_DRAM_CLK / 2;

	return DIV_ROUND_UP(ctrl_freq * nanoseconds, 1000);
}

void mctl_set_timing_params(const struct dram_para *para);
unsigned long sunxi_dram_init(void);