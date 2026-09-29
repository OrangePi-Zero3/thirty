/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <stdint.h>

#include <arm64.h>

#pragma once

#define readb_relaxed(a)     (*(volatile uint8_t *)(uintptr_t)(a))
#define writeb_relaxed(v, a) (*(volatile uint8_t *)(uintptr_t)(a) = (v))

#define readl_relaxed(a)     (*(volatile uint32_t *)(uintptr_t)(a))
#define writel_relaxed(v, a) (*(volatile uint32_t *)(uintptr_t)(a) = (v))

#define readl(c) ({ uint32_t __v = readl_relaxed(c); dmb(); __v; })
#define writel(v, c) ({ uint32_t __v = (v); dmb(); writel_relaxed(__v, c); __v; })

#define readb(c) ({ uint8_t __v = readb_relaxed(c); dmb(); __v; })
#define writeb(v, c) ({ uint8_t __v = (v); dmb(); writeb_relaxed(__v, c); __v; })

#define setbits_le32(addr, set) writel(readl(addr) | (set), addr)
#define clrbits_le32(addr, clear) writel(readl(addr) & ~(clear), addr)
#define clrsetbits_le32(addr, clear, set) writel((readl(addr) & ~(clear)) | (set), addr)
