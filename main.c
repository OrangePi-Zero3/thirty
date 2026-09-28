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

#include <printf.h>

#include <fel.h>

void main(void)
{
	timer_init();
	clock_init();
	uart_init();
	i2c_init();

	printf("Initialisation complete.\n");
	
	printf("thirty SPL - %s\n", BUILDID);
	printf("Turn on LED!\n");
	gpio_configure_pin(SUNXI_GPC(13), SUNXI_GPIO_OUTPUT);
	gpio_set_drive(SUNXI_GPC(13), SUNXI_DRIVE_L0);
	gpio_set_pin_state(SUNXI_GPC(13), 1);

	unsigned long dram_size = sunxi_dram_init();
	printf("DRAM size: %lu bytes\n", dram_size);

	// TODO: Return to FEL only if the device booted from FEL.
	printf("Returning to FEL mode, SP: 0x%x, LR: 0x%x\n", fel_stash.sp, fel_stash.lr);
	return_to_fel(fel_stash.sp, fel_stash.lr);

	while (1);
}