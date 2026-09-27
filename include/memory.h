/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#pragma once

#define writel(v, a) (*(volatile uint32_t *)(uintptr_t)(a) = (v))
#define readl(a) (*(volatile uint32_t *)(uintptr_t)(a))

#define setbits_le32(addr, set) writel(readl(addr) | (set), addr)
#define clrbits_le32(addr, clear) writel(readl(addr) & ~(clear), addr)
#define clrsetbits_le32(addr, clear, set) writel((readl(addr) & ~(clear)) | (set), addr)
