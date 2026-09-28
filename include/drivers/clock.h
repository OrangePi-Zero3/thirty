/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */
#include <stdint.h>

#include <bitops.h>

#pragma once

#define CCU_BASE                    0x03001000
#define CCU_H6_PLL1_CFG			    0x000
#define CCU_H6_PLL5_CFG			    0x010
#define CCU_H6_PLL6_CFG			    0x020
#define CCU_H6_CPU_AXI_CFG		    0x500
#define CCU_H6_PSI_AHB1_AHB2_CFG	0x510
#define CCU_H6_AHB3_CFG			    0x51c
#define CCU_H6_APB1_CFG			    0x520
#define CCU_H6_APB2_CFG			    0x524
#define CCU_H6_MBUS_CFG			    0x540
#define CCU_H6_DRAM_CLK_CFG		    0x800
#define CCU_H6_MBUS_GATE		    0x804
#define CCU_H6_DRAM_GATE_RESET		0x80c
#define CCU_NAND0_CLK_CFG		    0x810
#define CCU_NAND1_CLK_CFG		    0x814
#define CCU_H6_NAND_GATE_RESET		0x82c
#define CCU_MMC0_CLK_CFG		    0x830
#define CCU_MMC1_CLK_CFG		    0x834
#define CCU_MMC2_CLK_CFG		    0x838
#define CCU_H6_MMC_GATE_RESET		0x84c
#define CCU_H6_UART_GATE_RESET		0x90c
#define CCU_H6_I2C_GATE_RESET		0x91c

// Stuff for CCU_MMC0_CLK_CFG
#define CCM_MMC_CTRL_M(x)      ((x) - 1)
#define CCM_MMC_CTRL_N(x)      ((x) << 8)
#define CCM_MMC_CTRL_OSCM24    (0x0 << 24)
#define CCM_MMC_CTRL_PLL6      (0x1 << 24)
#define CCM_MMC_CTRL_ENABLE    (0x1 << 31)

#define PRCM_BASE                   0x07010000
#define CCU_PRCM_PLL_LDO_CFG		0x244
#define CCU_PRCM_SYS_PWROFF_GATING	0x250
#define CCU_PRCM_RES_CAL_CTRL		0x310
#define CCU_PRCM_OHMS240		    0x318

#define CLK_GATE                    (1 << 0)
#define CLK_RESET                   (1 << 16)
#define CCM_PLL_LOCK			    (1 << 28)

#define CCM_PLL_CTRL_EN			    (1 << 31)
#define CCM_PLL_LDO_EN			    (1 << 30)
#define CCM_PLL_LOCK_EN			    (1 << 29)
#define CCM_PLL_LOCK			    (1 << 28)
#define CCM_PLL_OUT_EN			    (1 << 27)
#define CCM_PLL1_UPDATE			    (1 << 26)
#define CCM_PLL1_CTRL_P(p)		    ((p) << 16)
#define CCM_PLL1_CTRL_N_MASK		GENMASK(15, 8)
#define CCM_PLL1_CTRL_N(n)		    (((n) - 1) << 8)

#define CCM_CPU_AXI_MUX_MASK		(0x3 << 24)
#define CCM_CPU_AXI_MUX_OSC24M		(0x0 << 24)
#define CCM_CPU_AXI_MUX_PLL_CPUX	(0x3 << 24)
#define CCM_CPU_AXI_APB_MASK		0x300
#define CCM_CPU_AXI_AXI_MASK		0x3
#define CCM_CPU_AXI_DEFAULT_FACTORS	0x301

#define MBUS_CLK_SRC_PLL6X2		    (1 << 24)
#define MBUS_CLK_M(m)			    (((m)-1) << 0)

#define CCM_PLL6_DEFAULT		    0xa8003100
#define CCM_PSI_AHB1_AHB2_DEFAULT	0x03000002
#define CCM_AHB3_DEFAULT		    0x03000002
#define CCM_APB1_DEFAULT		    0x03000102

// CLK ID
#define CLK_PLL_PERIPH0		4

#define CLK_CPUX		21

#define CLK_APB1		26

#define CLK_DE			29
#define CLK_BUS_DE		30
#define CLK_DEINTERLACE		31
#define CLK_BUS_DEINTERLACE	32
#define CLK_G2D			33
#define CLK_BUS_G2D		34
#define CLK_GPU0		35
#define CLK_BUS_GPU		36
#define CLK_GPU1		37
#define CLK_CE			38
#define CLK_BUS_CE		39
#define CLK_VE			40
#define CLK_BUS_VE		41
#define CLK_BUS_DMA		42
#define CLK_BUS_HSTIMER		43
#define CLK_AVS			44
#define CLK_BUS_DBG		45
#define CLK_BUS_PSI		46
#define CLK_BUS_PWM		47
#define CLK_BUS_IOMMU		48

#define CLK_MBUS_DMA		50
#define CLK_MBUS_VE		51
#define CLK_MBUS_CE		52
#define CLK_MBUS_TS		53
#define CLK_MBUS_NAND		54
#define CLK_MBUS_G2D		55

#define CLK_NAND0		57
#define CLK_NAND1		58
#define CLK_BUS_NAND		59
#define CLK_MMC0		60
#define CLK_MMC1		61
#define CLK_MMC2		62
#define CLK_BUS_MMC0		63
#define CLK_BUS_MMC1		64
#define CLK_BUS_MMC2		65
#define CLK_BUS_UART0		66
#define CLK_BUS_UART1		67
#define CLK_BUS_UART2		68
#define CLK_BUS_UART3		69
#define CLK_BUS_UART4		70
#define CLK_BUS_UART5		71
#define CLK_BUS_I2C0		72
#define CLK_BUS_I2C1		73
#define CLK_BUS_I2C2		74
#define CLK_BUS_I2C3		75
#define CLK_BUS_I2C4		76
#define CLK_SPI0		77
#define CLK_SPI1		78
#define CLK_BUS_SPI0		79
#define CLK_BUS_SPI1		80
#define CLK_EMAC_25M		81
#define CLK_BUS_EMAC0		82
#define CLK_BUS_EMAC1		83
#define CLK_TS			84
#define CLK_BUS_TS		85
#define CLK_BUS_THS		86
#define CLK_SPDIF		87
#define CLK_BUS_SPDIF		88
#define CLK_DMIC		89
#define CLK_BUS_DMIC		90
#define CLK_AUDIO_CODEC_1X	91
#define CLK_AUDIO_CODEC_4X	92
#define CLK_BUS_AUDIO_CODEC	93
#define CLK_AUDIO_HUB		94
#define CLK_BUS_AUDIO_HUB	95
#define CLK_USB_OHCI0		96
#define CLK_USB_PHY0		97
#define CLK_USB_OHCI1		98
#define CLK_USB_PHY1		99
#define CLK_USB_OHCI2		100
#define CLK_USB_PHY2		101
#define CLK_USB_OHCI3		102
#define CLK_USB_PHY3		103
#define CLK_BUS_OHCI0		104
#define CLK_BUS_OHCI1		105
#define CLK_BUS_OHCI2		106
#define CLK_BUS_OHCI3		107
#define CLK_BUS_EHCI0		108
#define CLK_BUS_EHCI1		109
#define CLK_BUS_EHCI2		110
#define CLK_BUS_EHCI3		111
#define CLK_BUS_OTG		112
#define CLK_BUS_KEYADC		113
#define CLK_HDMI		114
#define CLK_HDMI_SLOW		115
#define CLK_HDMI_CEC		116
#define CLK_BUS_HDMI		117
#define CLK_BUS_TCON_TOP	118
#define CLK_TCON_TV0		119
#define CLK_TCON_TV1		120
#define CLK_BUS_TCON_TV0	121
#define CLK_BUS_TCON_TV1	122
#define CLK_TVE0		123
#define CLK_BUS_TVE_TOP		124
#define CLK_BUS_TVE0		125
#define CLK_HDCP		126
#define CLK_BUS_HDCP		127
#define CLK_PLL_SYSTEM_32K	128
#define CLK_BUS_GPADC		129
#define CLK_TCON_LCD0		130
#define CLK_BUS_TCON_LCD0	131
#define CLK_TCON_LCD1		132
#define CLK_BUS_TCON_LCD1	133

#define RST_MBUS		0
#define RST_BUS_DE		1
#define RST_BUS_DEINTERLACE	2
#define RST_BUS_GPU		3
#define RST_BUS_CE		4
#define RST_BUS_VE		5
#define RST_BUS_DMA		6
#define RST_BUS_HSTIMER		7
#define RST_BUS_DBG		8
#define RST_BUS_PSI		9
#define RST_BUS_PWM		10
#define RST_BUS_IOMMU		11
#define RST_BUS_DRAM		12
#define RST_BUS_NAND		13
#define RST_BUS_MMC0		14
#define RST_BUS_MMC1		15
#define RST_BUS_MMC2		16
#define RST_BUS_UART0		17
#define RST_BUS_UART1		18
#define RST_BUS_UART2		19
#define RST_BUS_UART3		20
#define RST_BUS_UART4		21
#define RST_BUS_UART5		22
#define RST_BUS_I2C0		23
#define RST_BUS_I2C1		24
#define RST_BUS_I2C2		25
#define RST_BUS_I2C3		26
#define RST_BUS_I2C4		27
#define RST_BUS_SPI0		28
#define RST_BUS_SPI1		29
#define RST_BUS_EMAC0		30
#define RST_BUS_EMAC1		31
#define RST_BUS_TS		32
#define RST_BUS_THS		33
#define RST_BUS_SPDIF		34
#define RST_BUS_DMIC		35
#define RST_BUS_AUDIO_CODEC	36
#define RST_BUS_AUDIO_HUB	37
#define RST_USB_PHY0		38
#define RST_USB_PHY1		39
#define RST_USB_PHY2		40
#define RST_USB_PHY3		41
#define RST_BUS_OHCI0		42
#define RST_BUS_OHCI1		43
#define RST_BUS_OHCI2		44
#define RST_BUS_OHCI3		45
#define RST_BUS_EHCI0		46
#define RST_BUS_EHCI1		47
#define RST_BUS_EHCI2		48
#define RST_BUS_EHCI3		49
#define RST_BUS_OTG		50
#define RST_BUS_HDMI		51
#define RST_BUS_HDMI_SUB	52
#define RST_BUS_TCON_TOP	53
#define RST_BUS_TCON_TV0	54
#define RST_BUS_TCON_TV1	55
#define RST_BUS_TVE_TOP		56
#define RST_BUS_TVE0		57
#define RST_BUS_HDCP		58
#define RST_BUS_KEYADC		59
#define RST_BUS_GPADC		60
#define RST_BUS_TCON_LCD0	61
#define RST_BUS_TCON_LCD1	62
#define RST_BUS_LVDS		63

#define GATE(o, b)	{ .offset = (o), .bit = (b) }
#define GATE_DUMMY		{ .offset = 0, .bit = 0 }
#define RESET(o, b)	{ .offset = (o), .bit = (b) }

struct ccu_clk_gate {
    uint32_t offset;
    uint32_t bit;
};

struct ccu_reset {
    uint32_t offset;
    uint32_t bit;
};

void mmc_clk_init(int mmc_num);
void clock_set_pll1(unsigned int clk);
void clock_init(void);