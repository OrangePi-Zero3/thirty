/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <drivers/clock.h>

void mmc_init(int mmc_num) {
    mmc_clk_init(mmc_num);
}