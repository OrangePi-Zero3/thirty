/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */

#include <bitops.h>

#pragma once

#define MMC0_BASE                     0x04020000

// regs
#define MMC_GCTRL                     0x00
#define MMC_CLKCR                     0x04
#define MMC_TIMEOUT                   0x08
#define MMC_WIDTH                     0x0C
#define MMC_BLKSZ                     0x10
#define MMC_BYTECNT                   0x14
#define MMC_CMD                       0x18
#define MMC_ARG                       0x1C
#define MMC_RESP0                     0x20
#define MMC_RESP1                     0x24
#define MMC_RESP2                     0x28
#define MMC_RESP3                     0x2C
#define MMC_IMASK                     0x30
#define MMC_MINT                      0x34
#define MMC_RINT                      0x38
#define MMC_STATUS                    0x3C
#define MMC_FTRGLEVEL                 0x40
#define MMC_NTSR                      0x5C
#define MMC_SAMP_DL                   0x144   // H6 family only
#define MMC_FIFO                      0x200   // 0x100 on pre-H6 parts

// global control bits
#define GCTRL_SOFT_RESET              BIT(0)
#define GCTRL_FIFO_RESET              BIT(1)
#define GCTRL_DMA_RESET               BIT(2)
#define GCTRL_RESET                   (GCTRL_SOFT_RESET | GCTRL_FIFO_RESET | GCTRL_DMA_RESET)
#define GCTRL_DMA_ENABLE              BIT(5)
#define GCTRL_ACCESS_BY_AHB           BIT(31)

// clock stuff
#define CLK_POWERSAVE                 BIT(17)
#define CLK_ENABLE                    BIT(16)
#define CLK_DIVIDER_MASK              0xFF

// cmd flags
#define CMD_RESP_EXPIRE               BIT(6)
#define CMD_LONG_RESPONSE             BIT(7)
#define CMD_CHK_RESPONSE_CRC          BIT(8)
#define CMD_DATA_EXPIRE               BIT(9)
#define CMD_WRITE                     BIT(10)
#define CMD_AUTO_STOP                 BIT(12)
#define CMD_WAIT_PRE_OVER             BIT(13)
#define CMD_SEND_INIT_SEQ             BIT(15)
#define CMD_UPCLK_ONLY                BIT(21)
#define CMD_START                     BIT(31)

// response bits
#define RINT_RESP_ERROR               BIT(1)
#define RINT_COMMAND_DONE             BIT(2)
#define RINT_DATA_OVER                BIT(3)
#define RINT_RESP_CRC_ERROR           BIT(6)
#define RINT_DATA_CRC_ERROR           BIT(7)
#define RINT_RESP_TIMEOUT             BIT(8)
#define RINT_DATA_TIMEOUT             BIT(9)
#define RINT_VOLTAGE_CHANGE_DONE      BIT(10)
#define RINT_FIFO_RUN_ERROR           BIT(11)
#define RINT_HARD_WARE_LOCKED         BIT(12)
#define RINT_START_BIT_ERROR          BIT(13)
#define RINT_AUTO_COMMAND_DONE        BIT(14)
#define RINT_END_BIT_ERROR            BIT(15)

#define RINT_ERROR_MASK               (RINT_RESP_ERROR | RINT_RESP_CRC_ERROR | \
                                       RINT_DATA_CRC_ERROR | RINT_RESP_TIMEOUT | \
                                       RINT_DATA_TIMEOUT | RINT_VOLTAGE_CHANGE_DONE | \
                                       RINT_FIFO_RUN_ERROR | RINT_HARD_WARE_LOCKED | \
                                       RINT_START_BIT_ERROR | RINT_END_BIT_ERROR)

// status bits
#define STATUS_RXWL_FLAG              BIT(0)
#define STATUS_TXWL_FLAG              BIT(1)
#define STATUS_FIFO_EMPTY             BIT(2)
#define STATUS_FIFO_FULL              BIT(3)
#define STATUS_CARD_PRESENT           BIT(8)
#define STATUS_CARD_DATA_BUSY         BIT(9)
#define STATUS_FIFO_LEVEL(x)          (((x) >> 17) & 0x3FFF)

#define SAMP_DL_SW_EN                 BIT(7)

// response stuff
#define RESP_NONE                     0x00
#define RESP_R1                       0x01
#define RESP_R1B                      0x02
#define RESP_R2                       0x04
#define RESP_R3                       0x08
#define RESP_R6                       0x10
#define RESP_R7                       0x20

#define MMC_BLOCK_SIZE                512

// SD commands
#define CMD_GO_IDLE_STATE             0
#define CMD_ALL_SEND_CID              2
#define CMD_SEND_RELATIVE_ADDR        3
#define CMD_SWITCH_FUNC               6
#define CMD_SELECT_CARD               7
#define CMD_SEND_IF_COND              8
#define CMD_SEND_CSD                  9
#define CMD_STOP_TRANSMISSION         12
#define CMD_SET_BLOCKLEN              16
#define CMD_READ_SINGLE_BLOCK         17
#define CMD_READ_MULTIPLE_BLOCK       18
#define CMD_WRITE_SINGLE_BLOCK        24
#define CMD_WRITE_MULTIPLE_BLOCK      25
#define CMD_APP_CMD                   55

#define ACMD_SET_BUS_WIDTH            6
#define ACMD_SD_SEND_OP_COND          41

void mmc_init(int mmc_num);