/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#pragma once

#define MHz(x)	    ((x) * 1000000)
#define TIMER_FREQ	MHz(24)

void timer_init(void);
uint64_t timer_get_us(void);
void udelay(uint32_t us);