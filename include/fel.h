/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>
#pragma once

struct fel_stash {
	uint32_t sp;
	uint32_t lr;
	uint32_t cpsr;
	uint32_t sctlr;
	uint32_t vbar;
	uint32_t sp_irq;
	uint32_t icc_pmr;
	uint32_t icc_igrpen1;
};

extern struct fel_stash fel_stash;

void return_to_fel(uint32_t sp, uint32_t lr);