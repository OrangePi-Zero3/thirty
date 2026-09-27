/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#include <drivers/timer.h>

void timer_init(void)
{
	asm volatile("msr cntfrq_el0, %0" :: "r" (TIMER_FREQ));
	asm volatile("isb");
}

static inline uint64_t read_cntpct(void)
{
	uint64_t val;
	asm volatile("mrs %0, cntpct_el0" : "=r" (val));
	return val;
}

uint64_t timer_get_us(void)
{
	return read_cntpct() / (TIMER_FREQ / 1000000u);
}

void udelay(uint32_t us)
{
	uint64_t target = timer_get_us() + us;

	while (timer_get_us() < target)
		;
}