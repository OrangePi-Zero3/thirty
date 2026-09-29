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
    uint32_t number = GPIO_NUM(pin);
    uint32_t bit = GPIO_CFG_BIT(pin);
    uint32_t address = 0;
    
    switch (bank) {
        case SUNXI_GPIO_C:
            switch (number) {
                case 0 ... 7:
                    address = GPIO_CFG0_BASE(bank);
                    break;
                case 8 ... 15:
                    address = GPIO_CFG1_BASE(bank);
                    break;
                case 16:
                    address = GPIO_CFG2_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_F:
            switch (number) {
                case 0 ... 6:
                    address = GPIO_CFG0_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_G:
            switch (number) {
                case 0 ... 7:
                    address = GPIO_CFG0_BASE(bank);
                    break;
                case 8 ... 15:
                    address = GPIO_CFG1_BASE(bank);
                    break;
                case 16 ... 19:
                    address = GPIO_CFG2_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_H:
            switch (number) {
                case 0 ... 7:
                    address = GPIO_CFG0_BASE(bank);
                    break;
                case 8 ... 10:
                    address = GPIO_CFG1_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_I:
            switch (number) {
                case 0 ... 7:
                    address = GPIO_CFG0_BASE(bank);
                    break;
                case 8 ... 15:
                    address = GPIO_CFG1_BASE(bank);
                    break;
                case 16 ... 19:
                    address = GPIO_CFG2_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    
    if (!address)
        return;
    
    configuration = readl(address);
    configuration &= ~(0x7 << bit);
    configuration |= (config << bit);
    
    writel(configuration, address);
}

void gpio_set_drive (int pin, int level) {
    uint32_t configuration;
    uint32_t bank = GPIO_BANK(pin);
    uint32_t number = GPIO_NUM(pin);
    uint32_t bit = GPIO_MDR_BIT(pin);
    uint32_t address = 0;

    switch (bank) {
        case SUNXI_GPIO_C:
            switch (number) {
                case 0 ... 15:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                case 16:
                    address = GPIO_MDR1_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_F:
            switch (number) {
                case 0 ... 6:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_G:
            switch (number) {
                case 0 ... 15:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                case 16 ... 19:
                    address = GPIO_MDR1_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_H:
            switch (number) {
                case 0 ... 10:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_I:
            switch (number) {
                case 0 ... 15:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                case 16:
                    address = GPIO_MDR1_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    if (!address)
        return;

    configuration = readl(address);
    configuration &= ~(0x3 << bit);
    configuration |= (level << bit);

    writel(configuration, address);
}

void gpio_set_pin_pull(int pin, int level) {
    uint32_t configuration;
    uint32_t bank = GPIO_BANK(pin);
    uint32_t number = GPIO_NUM(pin);
    uint32_t index = GPIO_PULL_INDEX(pin);
    uint32_t offset = GPIO_PULL_OFFSET(pin);
    uint32_t address = GPIO_PULL_BASE(bank) + index;

    switch (bank) {
        case SUNXI_GPIO_C:
            switch (number) {
                case 0 ... 15:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                case 16:
                    address = GPIO_MDR1_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_F:
            switch (number) {
                case 0 ... 6:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_G:
            switch (number) {
                case 0 ... 15:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                case 16 ... 19:
                    address = GPIO_MDR1_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_H:
            switch (number) {
                case 0 ... 10:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        case SUNXI_GPIO_I:
            switch (number) {
                case 0 ... 15:
                    address = GPIO_MDR0_BASE(bank);
                    break;
                case 16:
                    address = GPIO_MDR1_BASE(bank);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    if (!address)
        return;

    configuration = readl(address);
    configuration &= ~(0x3 << offset);
    configuration |= (level << offset);

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