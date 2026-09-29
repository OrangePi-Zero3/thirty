/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#pragma once
#include <stdint.h>

#define IMG_MAGIC        0x7A7A1BEF
#define IMG_MAX_ENTRIES  4

#pragma pack(push, 1)
typedef struct {
    char name[16];
    uint64_t load_addr;
    uint64_t size;
    uint64_t offset_blocks;
} image_entry_t;

typedef struct {
    uint32_t magic;
    uint32_t cnt;
    image_entry_t entries[IMG_MAX_ENTRIES];
} image_header_t;
#pragma pack(pop)