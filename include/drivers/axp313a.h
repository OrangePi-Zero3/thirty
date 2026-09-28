/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#pragma once

#define AXP_CHIP_VERSION	0x3
#define AXP_CHIP_VERSION_MASK	0xc8
#define AXP_CHIP_ID		0x48

struct axp_reg_desc_spl {
	uint8_t	enable_reg;
	uint8_t	enable_mask;
	uint8_t	volt_reg;
	uint8_t	volt_mask;
	uint16_t	min_mV;
	uint16_t	max_mV;
	uint8_t	step_mV;
	uint8_t	split;
};

#define NA 0xff

int axp_set_dcdc1(unsigned int mvolt);
int axp_set_dcdc2(unsigned int mvolt);
int axp_set_dcdc3(unsigned int mvolt);
void pmic_init(void);
