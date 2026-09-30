/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#include <drivers/clock.h>
#include <drivers/gpio.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <drivers/dram.h>
#include <drivers/i2c.h>
#include <drivers/spi.h>
#include <drivers/axp313a.h>
#include <drivers/usb_power.h>
#include <drivers/mmc.h>
#include <drivers/nor_flash.h>

#include <boot.h>
#include <bootmode.h>

#include <printf.h>

void main(void)
{
	timer_init();
	clock_init();
	uart_init();
	i2c_init();
	spi_init();
	pmic_init();
	usb_power_init();
	spi_nor_init();
	mmc_init(0);

	// Speed up CPU to 1.008GHz
	printf("Speed up SoC...\n");
	clock_set_pll1(1008000000);

	printf("Initialisation complete.\n");

	printf("thirty SPL - %s\n", BUILDID);	
	printf("Turn on LED!\n");
	gpio_configure_pin(SUNXI_GPC(13), SUNXI_GPIO_OUTPUT);
	gpio_set_drive(SUNXI_GPC(13), SUNXI_DRIVE_L0);
	gpio_set_pin_state(SUNXI_GPC(13), 1);

	unsigned long dram_size = sunxi_dram_init();
	printf("DRAM size: %lu bytes\n", dram_size);

	load_and_boot_images(dram_size, sunxi_get_boot_source());

	while (1);
}
