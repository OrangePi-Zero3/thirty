/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>
#include <memory.h>
#include <drivers/clock.h>

void enable_clk(int clk) {
    uint32_t v = readl(clk);
    v |= CLK_GATE | CLK_RESET;
    writel(v, clk);
}