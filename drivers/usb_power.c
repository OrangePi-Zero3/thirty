/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <drivers/gpio.h>

void usb_power_init(void)
{
    // Must power up USB-A port for it to properly function (wow).
	gpio_configure_pin(SUNXI_GPC(16), SUNXI_GPIO_OUTPUT);
	gpio_set_drive(SUNXI_GPC(16), SUNXI_DRIVE_L0);
	gpio_set_pin_state(SUNXI_GPC(16), 1);
}