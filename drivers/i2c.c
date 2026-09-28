/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <drivers/timer.h>
#include <printf.h>
#include <memory.h>

// todo move this away
#define R_PIO_BASE            0x07022000
#define R_PIO_PL_CFG0         (R_PIO_BASE + 0x00)
#define R_PIO_PL_MUX_R_TWI    3

static int twsi_wait_iflg (void) {
    for (uint64_t elapsed = 0; elapsed < I2C_TIMEOUT_US; elapsed++) {
        if ((readl (TWI_CONTROL) & TWI_CTRL_IFLG) != 0) {
            return 0;
        }
        udelay(1);
    }

  return -1;
}

static int twsi_step (uint32_t control, uint32_t expected_status) {
  int status;
  uint32_t actual;

  writel (control, TWI_CONTROL);

  status = twsi_wait_iflg ();
  if (status != 0) {
    printf("I2C timeout waiting for IFLG (ctrl 0x%02x)\n", control);
    return status;
  }

  actual = readl (TWI_STATUS) & TWI_STAT_MASK;
  if (actual != expected_status) {
    printf("I2C bad state, expected 0x%02x got 0x%02x\n", expected_status, actual);
    return -2;
  }

  return 0;
}

static void twsi_stop (void)
{
  writel (TWI_CTRL_TWSIEN | TWI_CTRL_STOP | TWI_CTRL_CLEAR_IFLG, TWI_CONTROL);

  for (uint64_t elapsed = 0; elapsed < 1000; elapsed++) {
    if ((readl (TWI_CONTROL) & TWI_CTRL_STOP) == 0) {
      return;
    }

    udelay (1);
  }

  printf("I2C timeout waiting for STOP to clear\n");
}

int i2c_write (uint8_t addr, uint8_t reg, uint8_t value) {
  int status;

  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_START | TWI_CTRL_CLEAR_IFLG, TWI_STAT_START);
  if (status != 0) {
    return status;
  }

  writel ((addr << 1) | 0, TWI_DATA);
  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_CLEAR_IFLG, TWI_STAT_ADDR_W_ACK);
  if (status != 0) {
    return status;
  }

  writel (reg, TWI_DATA);
  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_CLEAR_IFLG, TWI_STAT_DATA_W_ACK);
  if (status != 0) {
    return status;
  }

  writel (value, TWI_DATA);
  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_CLEAR_IFLG, TWI_STAT_DATA_W_ACK);

  twsi_stop ();
  return status;
}

int i2c_read (uint8_t addr, uint8_t reg, uint8_t *value) {
  int status;

  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_START | TWI_CTRL_CLEAR_IFLG, TWI_STAT_START);
  if (status != 0) {
    return status;
  }

  writel ((addr << 1) | 0, TWI_DATA);
  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_CLEAR_IFLG, TWI_STAT_ADDR_W_ACK);
  if (status != 0) {
    return status;
  }

  writel (reg, TWI_DATA);
  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_CLEAR_IFLG, TWI_STAT_DATA_W_ACK);
  if (status != 0) {
    return status;
  }

  // Repeated START to turn the bus around without releasing it.
  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_START | TWI_CTRL_CLEAR_IFLG, TWI_STAT_RSTART);
  if (status != 0) {
    return status;
  }

  writel ((addr << 1) | 1, TWI_DATA);
  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_CLEAR_IFLG, TWI_STAT_ADDR_R_ACK);
  if (status != 0) {
    return status;
  }

  // Single byte, so NAK it to tell the device to stop driving.
  status = twsi_step (TWI_CTRL_TWSIEN | TWI_CTRL_CLEAR_IFLG, TWI_STAT_DATA_R_NAK);
  if (status != 0) {
    return status;
  }

  *value = (uint8_t)readl (TWI_DATA);

  twsi_stop ();
  return status;
}

// Just enough for AXP313
void i2c_init (void)
{
    uint32_t val;

    // Gate and reset live in one PRCM register; setting a reset bit releases it.
    val = readl (PRCM_TWI_GATE_RESET);
    val |= PRCM_TWI_GATE | PRCM_TWI_RESET;
    writel (val, PRCM_TWI_GATE_RESET);
    udelay (10);

    // Set pinmux
    val  = readl (R_PIO_PL_CFG0);
    val &= ~(0xFU << 0 | 0xFU << 4);
    val |= (R_PIO_PL_MUX_R_TWI << 0) | (R_PIO_PL_MUX_R_TWI << 4);
    writel (val, R_PIO_PL_CFG0);

    // Soft reset the controller, then set speed and enable it as a master.
    writel (0, TWI_SOFT_RESET);
    udelay (10);

    writel (TWI_BAUD_400K, TWI_BAUDRATE);
    writel (0, TWI_SLAVE_ADDR);
    writel (0, TWI_XTND_SLAVE_ADDR);
    writel (TWI_CTRL_TWSIEN, TWI_CONTROL);
    udelay (10);

    printf("I2C initialised\n");
}
