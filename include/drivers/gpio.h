/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#pragma once

#define H6_PIO_BASE			0x0300B000
#define H6_PIO_BANK_SIZE	0x24
#define H6_PIO_DAT_OFF		0x10
#define H6_PIO_PULL_OFF		0x1C


#define SUNXI_GPIO_A    0 // Unused
#define SUNXI_GPIO_B    1 // Unused
#define SUNXI_GPIO_C    2
#define SUNXI_GPIO_D    3 // Unused
#define SUNXI_GPIO_E    4 // Unused
#define SUNXI_GPIO_F    5
#define SUNXI_GPIO_G    6
#define SUNXI_GPIO_H    7
#define SUNXI_GPIO_I    9

#define GPIO_BANK(pin)		((pin) >> 5)
#define GPIO_NUM(pin)		((pin) & 0x1F)

#define GPIO_CFG0_BASE(bank)	((H6_PIO_BASE + (bank) * H6_PIO_BANK_SIZE))
#define GPIO_CFG1_BASE(bank)	((H6_PIO_BASE + (bank) * H6_PIO_BANK_SIZE + 0x4))
#define GPIO_CFG2_BASE(bank)	((H6_PIO_BASE + (bank) * H6_PIO_BANK_SIZE + 0x4))
#define GPIO_CFG3_BASE(bank)	((H6_PIO_BASE + (bank) * H6_PIO_BANK_SIZE + 0x4))
#define GPIO_CFG_BIT(pin)	    ((GPIO_NUM(pin) & 0x7) << 2)

#define GPIO_MDR0_BASE(bank)	((H6_PIO_BASE + (bank) * H6_PIO_BANK_SIZE + 0x14))
#define GPIO_MDR1_BASE(bank)	((H6_PIO_BASE + (bank) * H6_PIO_BANK_SIZE + 0x18))
#define GPIO_MDR_BIT(pin)	    (GPIO_NUM(pin) * 2)

#define GPIO_PULL_BASE(bank)	((H6_PIO_BASE + (bank) * H6_PIO_BANK_SIZE + H6_PIO_PULL_OFF))
#define GPIO_PULL_INDEX(pin)	(GPIO_NUM(pin) >> 4)
#define GPIO_PULL_OFFSET(pin)	((GPIO_NUM(pin) & 0xF) << 1)

#define GPIO_DAT_BASE(bank)	((H6_PIO_BASE + (bank) * H6_PIO_BANK_SIZE + H6_PIO_DAT_OFF))

/* GPIO bank sizes */
#define SUNXI_GPIO_A_NR    (32) // Unused
#define SUNXI_GPIO_B_NR    (32) // Unused
#define SUNXI_GPIO_C_NR    (32)
#define SUNXI_GPIO_D_NR    (32) // Unused
#define SUNXI_GPIO_E_NR    (32) // Unused
#define SUNXI_GPIO_F_NR    (32)
#define SUNXI_GPIO_G_NR    (32)
#define SUNXI_GPIO_H_NR    (32)
#define SUNXI_GPIO_I_NR    (32)

#define SUNXI_GPIO_NEXT(__gpio) ((__gpio##_START) + (__gpio##_NR) + 0)

enum sunxi_gpio_number {
	SUNXI_GPIO_A_START = 0, // Unused
	SUNXI_GPIO_B_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_A), // Unused
	SUNXI_GPIO_C_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_B), // Unused
	SUNXI_GPIO_D_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_C),
	SUNXI_GPIO_E_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_D), // Unused
	SUNXI_GPIO_F_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_E), // Unused
	SUNXI_GPIO_G_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_F),
	SUNXI_GPIO_H_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_G),
	SUNXI_GPIO_I_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_H),
};

/* SUNXI GPIO number definitions */
#define SUNXI_GPC(_nr)          (SUNXI_GPIO_C_START + (_nr))
#define SUNXI_GPF(_nr)          (SUNXI_GPIO_F_START + (_nr))
#define SUNXI_GPG(_nr)          (SUNXI_GPIO_G_START + (_nr))
#define SUNXI_GPH(_nr)          (SUNXI_GPIO_H_START + (_nr))
#define SUNXI_GPI(_nr)          (SUNXI_GPIO_I_START + (_nr))

/* GPIO pin function config */
#define SUNXI_GPIO_INPUT        (0)
#define SUNXI_GPIO_OUTPUT       (1)
#define MUX_2					(2)
#define MUX_3					(3)
#define MUX_4					(4)
#define MUX_5					(5)
#define MUX_6					(6)

/* GPIO Multi Drive Select */
#define SUNXI_DRIVE_L0		(0)
#define SUNXI_DRIVE_L1		(1)
#define SUNXI_DRIVE_L2		(2)
#define SUNXI_DRIVE_L3		(3)

/* GPIO pin pull-up/down config */
#define SUNXI_GPIO_PULL_DISABLE (0)
#define SUNXI_GPIO_PULL_UP      (1)
#define SUNXI_GPIO_PULL_DOWN    (2)

// Function prototypes
void gpio_configure_pin (int pin, int config);
void gpio_set_drive (int pin, int level);
void gpio_set_pin_pull(int pin, int level);
void gpio_set_pin_state (int pin, int enable);