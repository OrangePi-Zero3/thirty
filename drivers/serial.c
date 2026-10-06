/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#include <drivers/gpio.h>
#include <drivers/serial.h>
#include <drivers/clock.h>

#include <memory.h>

void uart_init(void)
{
    gpio_configure_pin(SUNXI_GPH(0), MUX_2);
    gpio_configure_pin(SUNXI_GPH(1), MUX_2);
    gpio_set_pin_pull(SUNXI_GPH(1), SUNXI_GPIO_PULL_UP);

    // 115200.
    writel(0x80, UART_LCR);
    writel(0, UART_DLH);
    writel(13, UART_DLL);
    writel(0x03, UART_LCR);
}

void uart_putc(char c)
{
    while (!(readl(UART_LSR) & UART_LSR_TEMT))
        ;

    if (c == '\n')
        uart_putc('\r');

    writel((uint32_t)(uint8_t)c, UART_THR);
}

void uart_getc(char *c)
{
    if(!(readl(UART_LSR) & UART_LSR_DR))
        return;

    *c = (char)(readl(UART_RBR) & 0xFF);
}

void uart_puts(char *s)
{
    while (*s)
        uart_putc(*s++);
}
