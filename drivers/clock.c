/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>
#include <stddef.h>

#include <drivers/clock.h>
#include <drivers/timer.h>

#include <bitops.h>

#include <memory.h>

#include <linux/kernel.h>

static struct ccu_clk_gate h616_gates[] = {
	[CLK_PLL_PERIPH0]	= GATE(0x020, BIT(31) | BIT(27)),

	[CLK_APB1]		= GATE_DUMMY,

	[CLK_DE]		= GATE(0x600, BIT(31)),
	[CLK_BUS_DE]		= GATE(0x60c, BIT(0)),

	[CLK_MBUS_NAND]		= GATE(0x804, BIT(5)),

	[CLK_NAND0]		= GATE(0x810, BIT(31)),
	[CLK_NAND1]		= GATE(0x814, BIT(31)),
	[CLK_BUS_NAND]		= GATE(0x82c, BIT(0)),

	[CLK_BUS_MMC0]		= GATE(0x84c, BIT(0)),
	[CLK_BUS_MMC1]		= GATE(0x84c, BIT(1)),
	[CLK_BUS_MMC2]		= GATE(0x84c, BIT(2)),

	[CLK_BUS_UART0]		= GATE(0x90c, BIT(0)),
	[CLK_BUS_UART1]		= GATE(0x90c, BIT(1)),
	[CLK_BUS_UART2]		= GATE(0x90c, BIT(2)),
	[CLK_BUS_UART3]		= GATE(0x90c, BIT(3)),
	[CLK_BUS_UART4]		= GATE(0x90c, BIT(4)),
	[CLK_BUS_UART5]		= GATE(0x90c, BIT(5)),

	[CLK_BUS_I2C0]		= GATE(0x91c, BIT(0)),
	[CLK_BUS_I2C1]		= GATE(0x91c, BIT(1)),
	[CLK_BUS_I2C2]		= GATE(0x91c, BIT(2)),
	[CLK_BUS_I2C3]		= GATE(0x91c, BIT(3)),
	[CLK_BUS_I2C4]		= GATE(0x91c, BIT(4)),

	[CLK_SPI0]		= GATE(0x940, BIT(31)),
	[CLK_SPI1]		= GATE(0x944, BIT(31)),

	[CLK_BUS_SPI0]		= GATE(0x96c, BIT(0)),
	[CLK_BUS_SPI1]		= GATE(0x96c, BIT(1)),

	[CLK_BUS_EMAC0]		= GATE(0x97c, BIT(0)),
	[CLK_BUS_EMAC1]		= GATE(0x97c, BIT(1)),

	[CLK_USB_PHY0]		= GATE(0xa70, BIT(29)),
	[CLK_USB_OHCI0]		= GATE(0xa70, BIT(31)),

	[CLK_USB_PHY1]		= GATE(0xa74, BIT(29)),
	[CLK_USB_OHCI1]		= GATE(0xa74, BIT(31)),

	[CLK_USB_PHY2]		= GATE(0xa78, BIT(29)),
	[CLK_USB_OHCI2]		= GATE(0xa78, BIT(31)),

	[CLK_USB_PHY3]		= GATE(0xa7c, BIT(29)),
	[CLK_USB_OHCI3]		= GATE(0xa7c, BIT(31)),

	[CLK_BUS_OHCI0]		= GATE(0xa8c, BIT(0)),
	[CLK_BUS_OHCI1]		= GATE(0xa8c, BIT(1)),
	[CLK_BUS_OHCI2]		= GATE(0xa8c, BIT(2)),
	[CLK_BUS_OHCI3]		= GATE(0xa8c, BIT(3)),
	[CLK_BUS_EHCI0]		= GATE(0xa8c, BIT(4)),
	[CLK_BUS_EHCI1]		= GATE(0xa8c, BIT(5)),
	[CLK_BUS_EHCI2]		= GATE(0xa8c, BIT(6)),
	[CLK_BUS_EHCI3]		= GATE(0xa8c, BIT(7)),
	[CLK_BUS_OTG]		= GATE(0xa8c, BIT(8)),

	[CLK_HDMI]		    = GATE(0xb00, BIT(31)),
	[CLK_HDMI_SLOW]		= GATE(0xb04, BIT(31)),
	[CLK_HDMI_CEC]		= GATE(0xb10, BIT(31)),
	[CLK_BUS_HDMI]		= GATE(0xb1c, BIT(0)),
	[CLK_BUS_TCON_TOP]	= GATE(0xb5c, BIT(0)),
	[CLK_TCON_TV0]		= GATE(0xb80, BIT(31)),
	[CLK_TCON_TV1]		= GATE(0xb84, BIT(31)),
	[CLK_BUS_TCON_TV0]	= GATE(0xb9c, BIT(0)),
	[CLK_BUS_TCON_TV1]	= GATE(0xb9c, BIT(1)),
};

static struct ccu_reset h616_resets[] = {
	[RST_BUS_DE]		= RESET(0x60c, BIT(16)),
	[RST_BUS_NAND]		= RESET(0x82c, BIT(16)),

	[RST_BUS_MMC0]		= RESET(0x84c, BIT(16)),
	[RST_BUS_MMC1]		= RESET(0x84c, BIT(17)),
	[RST_BUS_MMC2]		= RESET(0x84c, BIT(18)),

	[RST_BUS_UART0]		= RESET(0x90c, BIT(16)),
	[RST_BUS_UART1]		= RESET(0x90c, BIT(17)),
	[RST_BUS_UART2]		= RESET(0x90c, BIT(18)),
	[RST_BUS_UART3]		= RESET(0x90c, BIT(19)),
	[RST_BUS_UART4]		= RESET(0x90c, BIT(20)),
	[RST_BUS_UART5]		= RESET(0x90c, BIT(21)),

	[RST_BUS_I2C0]		= RESET(0x91c, BIT(16)),
	[RST_BUS_I2C1]		= RESET(0x91c, BIT(17)),
	[RST_BUS_I2C2]		= RESET(0x91c, BIT(18)),
	[RST_BUS_I2C3]		= RESET(0x91c, BIT(19)),
	[RST_BUS_I2C4]		= RESET(0x91c, BIT(20)),

	[RST_BUS_SPI0]		= RESET(0x96c, BIT(16)),
	[RST_BUS_SPI1]		= RESET(0x96c, BIT(17)),

	[RST_BUS_EMAC0]		= RESET(0x97c, BIT(16)),
	[RST_BUS_EMAC1]		= RESET(0x97c, BIT(17)),

	[RST_USB_PHY0]		= RESET(0xa70, BIT(30)),

	[RST_USB_PHY1]		= RESET(0xa74, BIT(30)),

	[RST_USB_PHY2]		= RESET(0xa78, BIT(30)),

	[RST_USB_PHY3]		= RESET(0xa7c, BIT(30)),

	[RST_BUS_OHCI0]		= RESET(0xa8c, BIT(16)),
	[RST_BUS_OHCI1]		= RESET(0xa8c, BIT(17)),
	[RST_BUS_OHCI2]		= RESET(0xa8c, BIT(18)),
	[RST_BUS_OHCI3]		= RESET(0xa8c, BIT(19)),
	[RST_BUS_EHCI0]		= RESET(0xa8c, BIT(20)),
	[RST_BUS_EHCI1]		= RESET(0xa8c, BIT(21)),
	[RST_BUS_EHCI2]		= RESET(0xa8c, BIT(22)),
	[RST_BUS_EHCI3]		= RESET(0xa8c, BIT(23)),
	[RST_BUS_OTG]		= RESET(0xa8c, BIT(24)),

	[RST_BUS_HDMI]		= RESET(0xb1c, BIT(16)),
	[RST_BUS_HDMI_SUB]	= RESET(0xb1c, BIT(17)),
	[RST_BUS_TCON_TOP]	= RESET(0xb5c, BIT(16)),
	[RST_BUS_TCON_TV0]	= RESET(0xb9c, BIT(16)),
	[RST_BUS_TCON_TV1]	= RESET(0xb9c, BIT(17)),
};

static void set_gate(struct ccu_clk_gate *gate, int enable)
{
    // Nothing is at the base so filter out dummys.
    if (!gate->offset)
        return;

    uint32_t v = readl(CCU_BASE + gate->offset);

    if (enable)
        v |= gate->bit;
    else
        v &= ~gate->bit;

    writel(v, CCU_BASE + gate->offset);
}

static void clock_set_reset(struct ccu_reset *reset, int enable)
{
    // Nothing is at the base so filter out dummys.
    if (!reset->offset)
        return;

    uint32_t v = readl(CCU_BASE + reset->offset);

    if (enable)
        v |= reset->bit;
    else
        v &= ~reset->bit;

    writel(v, CCU_BASE + reset->offset);
}

/* A shared routine to program the CPU PLLs for H6, H616, T113, A523 */
static void clock_set_pll(uint32_t *reg, unsigned int n)
{
	uint32_t val = readl(reg);

	/* clear the lock enable bit */
	val &= ~CCM_PLL_LOCK_EN;
	writel(val, reg);

	/* gate the output on the newer SoCs */
	val &= ~CCM_PLL_OUT_EN;
	writel(val, reg);

	val &= ~(CCM_PLL1_CTRL_N_MASK | GENMASK(3, 0) | GENMASK(21, 16));
	val |= CCM_PLL1_CTRL_N(n);
	writel(val, reg);			/* program parameter */

	val |= CCM_PLL_CTRL_EN;
	writel(val, reg);			/* enable PLL */

	val |= CCM_PLL_LOCK_EN;
	writel(val, reg);			/* start locking process */

	while (!(readl(reg) & CCM_PLL_LOCK)) {	/* wait for lock bit */
	}
	udelay(20);				/* wait as per manual */

	/* un-gate the output on the newer SoCs */
	val |= CCM_PLL_OUT_EN;
	writel(val, reg);
}

static void clock_h6_set_cpu_pll(unsigned int n_factor)
{
	void *const ccm = (void *)CCU_BASE;
	uint32_t val;

	/* Switch CPU clock source to 24MHz HOSC while changing the PLL */
	val = readl(ccm + CCU_H6_CPU_AXI_CFG);
	val &= ~CCM_CPU_AXI_MUX_MASK;
	val |= CCM_CPU_AXI_MUX_OSC24M;
	writel(val, ccm + CCU_H6_CPU_AXI_CFG);

	clock_set_pll(ccm + CCU_H6_PLL1_CFG, n_factor);

	/* Switch CPU clock source to the CPU PLL */
	val = readl(ccm + CCU_H6_CPU_AXI_CFG);
	val &= ~CCM_CPU_AXI_MUX_MASK;
	val |= CCM_CPU_AXI_MUX_PLL_CPUX;
	writel(val, ccm + CCU_H6_CPU_AXI_CFG);
}

void clock_set_pll1(unsigned int clk)
{
	/* Do not support clocks < 288MHz as they need factor P */
	if (clk < 288000000)
		clk = 288000000;

	clk /= 24000000;

	clock_h6_set_cpu_pll(clk);
}

static void clock_init_safe(void)
{
	void *const ccm = (void *)CCU_BASE;
	void *const prcm = (void *)PRCM_BASE;
 
	setbits_le32(prcm + CCU_PRCM_SYS_PWROFF_GATING, 0x10);
	udelay(1);
 
	setbits_le32(prcm + CCU_PRCM_RES_CAL_CTRL, 2);
	udelay(1);
 
	clrbits_le32(prcm + CCU_PRCM_RES_CAL_CTRL, 1);
	udelay(1);
	setbits_le32(prcm + CCU_PRCM_RES_CAL_CTRL, 1);
 
    // Not sure yet if this is needed for H616, but it is in H6 code.
	// if (IS_ENABLED(CONFIG_MACH_SUN50I_H6)) {
	// 	/* set key field for ldo enable */
	// 	setbits_le32(prcm + CCU_PRCM_PLL_LDO_CFG, 0xA7000000);
	// 	/* set PLL VDD LDO output to 1.14 V */
	// 	setbits_le32(prcm + CCU_PRCM_PLL_LDO_CFG, 0x60000);
	// }
 
	clock_set_pll1(408000000);
 
	writel(CCM_PLL6_DEFAULT, ccm + CCU_H6_PLL6_CFG);
	while (!(readl(ccm + CCU_H6_PLL6_CFG) & CCM_PLL_LOCK))
		;
 
	clrsetbits_le32(ccm + CCU_H6_CPU_AXI_CFG,
			CCM_CPU_AXI_APB_MASK | CCM_CPU_AXI_AXI_MASK,
			CCM_CPU_AXI_DEFAULT_FACTORS);
 
	writel(CCM_PSI_AHB1_AHB2_DEFAULT, ccm + CCU_H6_PSI_AHB1_AHB2_CFG);
	writel(CCM_AHB3_DEFAULT, ccm + CCU_H6_AHB3_CFG);
	writel(CCM_APB1_DEFAULT, ccm + CCU_H6_APB1_CFG);
 
	/*
	 * The mux and factor are set, but the clock will be enabled in
	 * DRAM initialization code.
	 */
	writel(MBUS_CLK_SRC_PLL6X2 | MBUS_CLK_M(3),
	       ccm + CCU_H6_MBUS_CFG);
}

void mmc_clk_init(int mmc_num) {
    int offset = CCU_MMC0_CLK_CFG + (mmc_num * 4);
    writel(CCM_MMC_CTRL_ENABLE | CCM_MMC_CTRL_OSCM24, CCU_BASE + offset);
}

void clock_init(void) {
    // Do safe init first.
    clock_init_safe();

    // Bulk enable all clocks and resets.
    for (size_t i = 0; i < ARRAY_SIZE(h616_gates); i++)
        set_gate(&h616_gates[i], 1);

    for (size_t i = 0; i < ARRAY_SIZE(h616_resets); i++)
        clock_set_reset(&h616_resets[i], 1);
}