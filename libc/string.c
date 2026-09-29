/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */
#include <stddef.h>

void* memcpy(void* destination, void* source, size_t num)
{
	int i;
	char* d = destination;
	char* s = source;
	for (i = 0; i < num; i++) {
		d[i] = s[i];
	}
	return destination;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
	const unsigned char* p1 = ptr1;
	const unsigned char* p2 = ptr2;
	for (size_t i = 0; i < num; i++) {
		if (p1[i] != p2[i]) {
			return (int)p1[i] - (int)p2[i];
		}
	}
	return 0;
}