/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#pragma once
#define UART_BASE      0x05000000

#define UART_RBR       (UART_BASE + 0x00)
#define UART_THR       (UART_BASE + 0x00)
#define UART_DLL       (UART_BASE + 0x00)
#define UART_DLH       (UART_BASE + 0x04)
#define UART_LCR       (UART_BASE + 0x0c)
#define UART_LSR       (UART_BASE + 0x14)

#define UART_LSR_DR    (1 << 0)
#define UART_LSR_TEMT  (1 << 6)

void uart_init(void);
void uart_putc(char c);
void uart_getc(char *c);
void uart_puts(char *s);
