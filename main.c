/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>
#include <drivers/gpio.h>
#include <drivers/serial.h>

#include <printf.h>

#include <fel.h>

void main(void)
{
	uart_init();
	
	printf("thirty SPL - %s\n", BUILDID);
	printf("Turn on LED!\n");
	gpio_configure_pin(SUNXI_GPC(13), SUNXI_GPIO_OUTPUT);
	gpio_set_drive(SUNXI_GPC(13), SUNXI_DRIVE_L0);
	gpio_set_pin_state(SUNXI_GPC(13), 1);

	// TODO: Return to FEL only if the device booted from FEL.
	printf("Returning to FEL mode, SP: 0x%x, LR: 0x%x\n", fel_stash.sp, fel_stash.lr);
	return_to_fel(fel_stash.sp, fel_stash.lr);
}