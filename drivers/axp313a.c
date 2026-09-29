/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */
#include <stddef.h>

#include <bitops.h>
#include <printf.h>

#include <drivers/axp313a.h>
#include <drivers/i2c.h>

#include <linux/kernel.h>

static const struct axp_reg_desc_spl axp_spl_dcdc_regulators[] = {
	{ 0x10, BIT(0), 0x13, 0x7f,  500, 1540,  10, 70 },
	{ 0x10, BIT(1), 0x14, 0x7f,  500, 1540,  10, 70 },
	{ 0x10, BIT(2), 0x15, 0x7f,  500, 1840,  10, 70 },
};

int pmic_bus_write(uint8_t reg, uint8_t value)
{
    return i2c_write(0x36, reg, value);
}

int pmic_bus_read(uint8_t reg, uint8_t *value)
{
    return i2c_read(0x36, reg, value);
}

int pmic_bus_setbits(uint8_t reg, uint8_t mask)
{
    int ret;
    uint8_t value;

    ret = pmic_bus_read(reg, &value);
    if (ret)
        return ret;

    value |= mask;
    return pmic_bus_write(reg, value);
}

int pmic_bus_clrbits(uint8_t reg, uint8_t mask)
{
    int ret;
    uint8_t value;

    ret = pmic_bus_read(reg, &value);
    if (ret)
        return ret;

    value &= ~mask;
    return pmic_bus_write(reg, value);
}

static uint8_t axp_mvolt_to_cfg(int mvolt, const struct axp_reg_desc_spl *reg)
{
	if (mvolt < reg->min_mV)
		mvolt = reg->min_mV;
	else if (mvolt > reg->max_mV)
		mvolt = reg->max_mV;

	mvolt -= reg->min_mV;

	/* voltage in the first range ? */
	if (mvolt <= reg->split * reg->step_mV)
		return mvolt / reg->step_mV;

	mvolt -= reg->split * reg->step_mV;

	return reg->split + mvolt / (reg->step_mV * 2);
}

static int axp_set_dcdc(size_t dcdc_num, unsigned int mvolt)
{
	const struct axp_reg_desc_spl *reg;
	int ret;

	if (dcdc_num < 1 || dcdc_num > ARRAY_SIZE(axp_spl_dcdc_regulators))
		return -1;

	reg = &axp_spl_dcdc_regulators[dcdc_num - 1];

	if (mvolt == 0)
		return pmic_bus_clrbits(reg->enable_reg, reg->enable_mask);

	ret = pmic_bus_write(reg->volt_reg, axp_mvolt_to_cfg(mvolt, reg));
	if (ret)
		return ret;

	return pmic_bus_setbits(reg->enable_reg, reg->enable_mask);
}

int axp_set_dcdc1(unsigned int mvolt)
{
	return axp_set_dcdc(1, mvolt);
}

int axp_set_dcdc2(unsigned int mvolt)
{
	return axp_set_dcdc(2, mvolt);
}

int axp_set_dcdc3(unsigned int mvolt)
{
	return axp_set_dcdc(3, mvolt);
}

void pmic_init(void)
{
    uint8_t chip_ver = 0;
    int failed = 0;

    i2c_read(0x36, AXP_CHIP_VERSION, &chip_ver);
    if ((chip_ver & AXP_CHIP_VERSION_MASK) != AXP_CHIP_ID) {
        printf("This is not AXP pmic! Chip Ver: 0x%02x\n", chip_ver);
        return;
    }

    printf("Setting voltage for DCDC2 to 1.0V\n");
    failed |= axp_set_dcdc2(1000);

    printf("Setting voltage for DCDC3 to 1.1V\n");
    failed |= axp_set_dcdc3(1100);

    if (failed)
        printf("Failed to set voltages\n");

    printf("PMIC initialisation complete\n");
}
