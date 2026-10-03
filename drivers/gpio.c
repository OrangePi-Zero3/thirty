/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#include <memory.h>
#include <drivers/gpio.h>

void gpio_configure_pin (int pin, int config) {
    uint32_t configuration;
    uint32_t bank = GPIO_BANK(pin);
    uint32_t bit = GPIO_CFG_BIT(pin);
    uint32_t address = GPIO_CFG0_BASE(bank) + (GPIO_NUM(pin) >> 3) * 4;

    configuration = readl(address);
    configuration &= ~(0xF << bit);
    configuration |= ((uint32_t)(config & 0xF) << bit);

    writel(configuration, address);
}

void gpio_set_drive (int pin, int level) {
    uint32_t configuration;
    uint32_t bank = GPIO_BANK(pin);
    uint32_t bit = GPIO_MDR_BIT(pin);
    uint32_t address = GPIO_MDR0_BASE(bank) + (GPIO_NUM(pin) >> 4) * 4;

    configuration = readl(address);
    configuration &= ~(0x3 << bit);
    configuration |= ((uint32_t)(level & 0x3) << bit);

    writel(configuration, address);
}

void gpio_set_pin_pull (int pin, int level) {
    uint32_t configuration;
    uint32_t bank = GPIO_BANK(pin);
    uint32_t offset = GPIO_PULL_OFFSET(pin);
    uint32_t address = GPIO_PULL_BASE(bank) + GPIO_PULL_INDEX(pin) * 4;

    configuration = readl(address);
    configuration &= ~(0x3 << offset);
    configuration |= ((uint32_t)(level & 0x3) << offset);

    writel(configuration, address);
}

void gpio_set_pin_state (int pin, int enable) {
    uint32_t configuration;
    uint32_t bank = GPIO_BANK(pin);
    uint32_t number = GPIO_NUM(pin);
    uint32_t address = GPIO_DAT_BASE(bank);

    configuration = readl(address);
    if (enable)
        configuration |= (1 << number);
    else
        configuration &= ~(1 << number);

    writel(configuration, address);
}

int gpio_get_pin_state (int pin) {
    uint32_t configuration;
    uint32_t bank = GPIO_BANK(pin);
    uint32_t number = GPIO_NUM(pin);
    uint32_t address = GPIO_DAT_BASE(bank);

    configuration = readl(address);
    return (configuration >> number) & 0x1;
}