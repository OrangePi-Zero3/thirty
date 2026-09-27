/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#pragma once
#include <generated/autoconf.h>

#if defined(CONFIG_CONSOLE_OUT)
int printf(volatile const char *fmt, ...);
#else
#define printf(fmt, ...) do { } while (0)
#endif