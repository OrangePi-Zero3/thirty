/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#pragma once
#define CCU_BASE        0x03001000
#define UART_BGR_REG    (CCU_BASE + 0x90c)

#define CLK_GATE        (1 << 0)
#define CLK_RESET       (1 << 16)

void enable_clk(int clk);