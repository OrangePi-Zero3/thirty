/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#pragma once
#include <stdint.h>
#include <bitops.h>

#define PRCM_TWI_GATE_RESET   0x0701019C
#define PRCM_TWI_GATE         BIT(0)
#define PRCM_TWI_RESET        BIT(16)

#define R_TWI_BASE            0x07081400
#define TWI_SLAVE_ADDR        (R_TWI_BASE + 0x00)
#define TWI_XTND_SLAVE_ADDR   (R_TWI_BASE + 0x04)
#define TWI_DATA              (R_TWI_BASE + 0x08)
#define TWI_CONTROL           (R_TWI_BASE + 0x0C)
#define TWI_STATUS            (R_TWI_BASE + 0x10)
#define TWI_BAUDRATE          (R_TWI_BASE + 0x14)
#define TWI_SOFT_RESET        (R_TWI_BASE + 0x18)

//
// f = 24MHz / (10 * (m+1) * 2^n), register is (m << 3) | n. m=8,n=4 gives
// 375kHz
//
#define TWI_BAUD_400K         0x44

#define TWI_CTRL_ACK          BIT(2)
#define TWI_CTRL_IFLG         BIT(3)
#define TWI_CTRL_STOP         BIT(4)
#define TWI_CTRL_START        BIT(5)
#define TWI_CTRL_TWSIEN       BIT(6)

#define TWI_CTRL_CLEAR_IFLG   BIT(3)

#define TWI_STAT_START        0x08
#define TWI_STAT_RSTART       0x10
#define TWI_STAT_ADDR_W_ACK   0x18
#define TWI_STAT_DATA_W_ACK   0x28
#define TWI_STAT_ADDR_R_ACK   0x40
#define TWI_STAT_DATA_R_NAK   0x58
#define TWI_STAT_IDLE         0xF8
#define TWI_STAT_MASK         0xF8

#define I2C_TIMEOUT_US        100000

void i2c_init(void);
int i2c_write(uint8_t addr, uint8_t reg, uint8_t value);
int i2c_read(uint8_t addr, uint8_t reg, uint8_t *value);
