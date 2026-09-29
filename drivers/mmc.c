/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, Umer Uddin <umer.uddin@mentallysanemainliners.org>
 */
#include <stdbool.h>
#include <stdint.h>

#include <drivers/clock.h>
#include <drivers/gpio.h>
#include <drivers/timer.h>
#include <drivers/mmc.h>

#include <memory.h>

#include <printf.h>

bool hc_card = false;
uint32_t rca = 0;
uint64_t blocks = 0;

static int mmc_update_clock(void) {
    writel(CMD_START | CMD_UPCLK_ONLY | CMD_WAIT_PRE_OVER, MMC0_BASE + MMC_CMD);

    for (int i = 0; i < 2000000; i++) {
        if ((readl(MMC0_BASE + MMC_CMD) & CMD_START) == 0) {
            writel(readl(MMC0_BASE + MMC_RINT), MMC0_BASE + MMC_RINT);
            return 0;
        }

        udelay(1);
    }

    printf("mmc_update_clock: timeout\n");

    return -1;
}

static int mmc_set_clock(int mmc_num, unsigned int hz) {
    int n, clk_cr, div;

    div = 24000000 / hz;
    if ((24000000 % hz) != 0) {
      div++;
    }

    n = 0;
    while (div > 16) {
      n++;
      div = (div + 1) / 2;
    }

    if (n > 3) {
      return -1;
    }

    clk_cr = readl(MMC0_BASE + MMC_CLKCR) & ~(1 << 16);
    writel(clk_cr, MMC0_BASE + MMC_CLKCR);

    if (mmc_update_clock() != 0) {
        printf("mmc_set_clock: mmc_update_clock failed\n");
        return -1;
    }

    writel(CCM_MMC_CTRL_ENABLE | CCM_MMC_CTRL_OSCM24 | CCM_MMC_CTRL_N (n) | CCM_MMC_CTRL_M (div), CCU_BASE + CCU_MMC0_CLK_CFG);

    clk_cr &= ~CLK_DIVIDER_MASK;
    writel(clk_cr, MMC0_BASE + MMC_CLKCR);

    writel(SAMP_DL_SW_EN, MMC0_BASE + MMC_SAMP_DL);

    clk_cr |= (1 << 16);
    writel(clk_cr, MMC0_BASE + MMC_CLKCR);

    return mmc_update_clock();
}

static int mmc_wait_rint(int done_bit, int timeout) {
    for (int i = 0; i < timeout; i++) {
        uint32_t rint = readl(MMC0_BASE + MMC_RINT);
        if((rint & RINT_ERROR_MASK) != 0) {
            printf("mmc_wait_rint: error, rint=0x%08x\n", rint);
            return -1;
        }

        if ((rint & done_bit) != 0) {
            return 0;
        }

        udelay(1);
    }

    printf("mmc_wait_rint: timeout\n");
    return -1;
}

static int mmc_transfer_pio(uint32_t *buffer, uint64_t cnt, bool read) {
    uint64_t idx = 0;
    uint32_t status = 0;

    writel(readl(MMC0_BASE + MMC_GCTRL) | GCTRL_ACCESS_BY_AHB, MMC0_BASE + MMC_GCTRL);

    while (idx < cnt) {
        for (int i = 0; ; i++) {
            status = readl(MMC0_BASE + MMC_STATUS);
            if((status & (read ? STATUS_FIFO_EMPTY : STATUS_FIFO_FULL)) == 0) {
                break;
            }

            if (i > 2000000) {
                printf("mmc_transfer_pio: fifo stall, idx=%llu\n", idx);
                return -1;
            }

            udelay(1);
        }

        if (!read) {
            writel(buffer[idx++], MMC0_BASE + MMC_FIFO);
            continue;
        }

        uint64_t in_fifo = STATUS_FIFO_LEVEL(status);
        if (in_fifo == 0)
            in_fifo = 1;

        while (in_fifo > 0 && idx < cnt) {
            buffer[idx++] = readl(MMC0_BASE + MMC_FIFO);
            in_fifo--;
        }
    }

    return 0;
}

void mmc_wait_not_busy(void) {
    for (int i = 0; i < 500000; i++) {
        if ((readl(MMC0_BASE + MMC_STATUS) & STATUS_CARD_DATA_BUSY) == 0)
            return;

        udelay(1);
    }

    printf("mmc_wait_not_busy: timeout\n");
}

static int mmc_send_command(uint32_t index, uint32_t argument, uint32_t resp_type,
                             void *buffer, uint64_t blocks, bool write,
                             uint32_t *response)
{
    uint32_t cmd_val;
    int i;

    mmc_wait_not_busy();

    writel(0xFFFFFFFF, MMC0_BASE + MMC_RINT);

    cmd_val = CMD_START | index;

    if (index == CMD_GO_IDLE_STATE)
        cmd_val |= CMD_SEND_INIT_SEQ;

    if (resp_type != RESP_NONE)
        cmd_val |= CMD_RESP_EXPIRE;

    if (resp_type == RESP_R2)
        cmd_val |= CMD_LONG_RESPONSE;

    if (resp_type != RESP_NONE && resp_type != RESP_R3)
        cmd_val |= CMD_CHK_RESPONSE_CRC;

    if (buffer != nullptr) {
        cmd_val |= CMD_DATA_EXPIRE | CMD_WAIT_PRE_OVER;

        if (write)
            cmd_val |= CMD_WRITE;

        if (blocks > 1)
            cmd_val |= CMD_AUTO_STOP;

        writel(MMC_BLOCK_SIZE, MMC0_BASE + MMC_BLKSZ);
        writel((uint32_t)(blocks * MMC_BLOCK_SIZE), MMC0_BASE + MMC_BYTECNT);
    }

    writel(argument, MMC0_BASE + MMC_ARG);
    writel(cmd_val, MMC0_BASE + MMC_CMD);

    if (buffer != nullptr) {
        if (mmc_transfer_pio((uint32_t *)buffer,
                              (blocks * MMC_BLOCK_SIZE) / sizeof(uint32_t),
                              !write) != 0)
            goto fail;
    }

    if (mmc_wait_rint(RINT_COMMAND_DONE, 1000000) != 0)
        goto fail;

    if (buffer != nullptr) {
        if (mmc_wait_rint(blocks > 1 ? RINT_AUTO_COMMAND_DONE : RINT_DATA_OVER,
                           2000000) != 0)
            goto fail;
    }

    if (resp_type == RESP_R1B) {
        for (i = 0; ; i++) {
            if ((readl(MMC0_BASE + MMC_STATUS) & STATUS_CARD_DATA_BUSY) == 0)
                break;

            if (i >= 2000000)
                goto fail;

            udelay(1);
        }
    }

    if (response != nullptr) {
        if (resp_type == RESP_R2) {
            response[0] = readl(MMC0_BASE + MMC_RESP3);
            response[1] = readl(MMC0_BASE + MMC_RESP2);
            response[2] = readl(MMC0_BASE + MMC_RESP1);
            response[3] = readl(MMC0_BASE + MMC_RESP0);
        } else {
            response[0] = readl(MMC0_BASE + MMC_RESP0);
        }
    }

    writel(0xFFFFFFFF, MMC0_BASE + MMC_RINT);
    writel(readl(MMC0_BASE + MMC_GCTRL) | GCTRL_FIFO_RESET, MMC0_BASE + MMC_GCTRL);
    printf("mmc_send_command: CMD%u done\n", index);
    return 0;

fail:
    printf("mmc_send_command: CMD%u failed rint 0x%08x status 0x%08x\n",
           index, readl(MMC0_BASE + MMC_RINT), readl(MMC0_BASE + MMC_STATUS));


    writel(GCTRL_RESET, MMC0_BASE + MMC_GCTRL);

    for (i = 0; i < 100000; i++) {
        if ((readl(MMC0_BASE + MMC_GCTRL) & GCTRL_RESET) == 0)
            break;

        udelay(1);
    }

    writel(0xFFFFFFFF, MMC0_BASE + MMC_RINT);
    mmc_update_clock();
    return -1;
}

static int mmc_controller_init(void) {
    // Configure pins
    for (int pin = 0; pin <= 5; pin++) {
        gpio_configure_pin(SUNXI_GPF(pin), MUX_2);
        gpio_set_pin_pull(SUNXI_GPF(pin), SUNXI_GPIO_PULL_UP);
    }

    writel(GCTRL_RESET, MMC0_BASE + MMC_GCTRL);
    udelay(1000);
    if ((readl(MMC0_BASE + MMC_GCTRL) & GCTRL_RESET) != 0) {
        printf("mmc_controller_init: reset failed\n");
        return -1;
    }
    else {
        printf("mmc_controller_init: reset done\n");
    }

    writel(0xFFFFFFFF, MMC0_BASE + MMC_RINT);
    writel(0, MMC0_BASE + MMC_WIDTH);

    return mmc_set_clock(0, 400000);
}

static int card_init(void) {
    bool v2_card = false;
    uint32_t response[4];
    uint32_t ocr = 0x00FF8000;
    uint32_t csd[4];
    int i = 0;

    if (mmc_send_command(CMD_GO_IDLE_STATE, 0, RESP_NONE, nullptr, 0, false, nullptr) != 0)
    {
        printf("card_init: CMD_GO_IDLE_STATE failed\n");
        return -1;
    }

    udelay(2000);

    if(mmc_send_command(CMD_SEND_IF_COND, 0x1AA, RESP_R7, nullptr, 0, false, response) == 0) {
        if ((response[0] & 0xFF) == 0xAA) {
            printf("SD 2.0 card detected\n");
            v2_card = true;
        }
    }

    if (v2_card)
        ocr |= (1 << 30);

    for (i = 0; i < 1000; i++) {
        if (mmc_send_command(CMD_APP_CMD, 0, RESP_R1, nullptr, 0, false, nullptr) != 0) {
            printf("card_init: CMD_APP_CMD failed\n");
            return -1;
        }

        if (mmc_send_command(ACMD_SD_SEND_OP_COND, ocr, RESP_R3, nullptr, 0, false, response) != 0) {
            printf("card_init: ACMD_SD_SEND_OP_COND failed\n");
            return -1;
        }

        if (response[0] & (1 << 31)) {
            printf("card_init: card ready\n");
            break;
        }

        udelay(1000);
    }

    if (i == 1000) {
        printf("card_init: failed to exit power up state\n");
        return -1;
    }

    hc_card = (response[0] & (1 << 30)) != 0;

    if (mmc_send_command(CMD_ALL_SEND_CID, 0, RESP_R2, nullptr, 0, false, response) != 0) {
        printf("card_init: CMD_ALL_SEND_CID failed\n");
        return -1;
    }

    if (mmc_send_command(CMD_SEND_RELATIVE_ADDR, 0, RESP_R6, nullptr, 0, false, response) != 0) {
        printf("card_init: CMD_SEND_RELATIVE_ADDR failed\n");
        return -1;
    }

    rca = response[0] & 0xFFFF0000;

    if (mmc_send_command(CMD_SEND_CSD, rca, RESP_R2, nullptr, 0, false, csd) != 0) {
        printf("card_init: CMD_SEND_CSD failed\n");
        return -1;
    }

    if (((csd[0] >> 30) & 0x3) == 1) {
        uint32_t c_size = ((csd[1] & 0x3F) << 16) | ((csd[2] & 0xFFFF0000) >> 16);

        blocks = (c_size + 1) * 1024;
    }
    else {
        uint32_t c_size = ((csd[1] & 0x3FF) << 2) | ((csd[2] & 0xC0000000) >> 30);
        uint32_t c_size_mult = ((csd[2] >> 15) & 0x7);
        uint32_t read_bl_len = (csd[2] >> 16) & 0xF;

        blocks = ((uint64_t)c_size + 1) * (1 << (c_size_mult + 2));
        blocks = (blocks << read_bl_len) / MMC_BLOCK_SIZE;
    }

    if (mmc_send_command(CMD_SELECT_CARD, rca, RESP_R1B, nullptr, 0, false, nullptr) != 0) {
        printf("card_init: CMD_SELECT_CARD failed\n");
        return -1;
    }

    if (mmc_send_command(CMD_APP_CMD, rca, RESP_R1, nullptr, 0, false, nullptr) == 0) {
        if (mmc_send_command(ACMD_SET_BUS_WIDTH, 2, RESP_R1, nullptr, 0, false, nullptr) == 0) {
            writel(1, MMC0_BASE + MMC_WIDTH);
            printf("card_init: bus width set to 4 bits\n");
        }
    }

    if (mmc_send_command(CMD_SET_BLOCKLEN, MMC_BLOCK_SIZE, RESP_R1, nullptr, 0, false, nullptr) != 0) {
        printf("card_init: CMD_SET_BLOCKLEN failed\n");
        return -1;
    }

    if (mmc_set_clock(0, 24000000) != 0) {
        printf("card_init: failed to set clock to 24MHz\n");
        return -1;
    }

    printf("card_init: Initialisation completed, detected %s card with %d blocks\n", hc_card ? "High Capacity" : "Standard Capacity", blocks);
}

int mmc_read_blocks(uint64_t start_block, uint64_t block_count, void *buffer) {
    uint32_t cmd = block_count > 1 ? CMD_READ_MULTIPLE_BLOCK : CMD_READ_SINGLE_BLOCK;
    uint32_t addr = hc_card ? start_block : start_block * MMC_BLOCK_SIZE;

    if (start_block + block_count > blocks) {
        printf("mmc_read_blocks: out of bounds\n");
        return -1;
    }

    if (mmc_send_command(cmd, addr, RESP_R1, buffer, block_count, false, nullptr) != 0) {
        printf("mmc_read_blocks: failed to read blocks\n");
        return -1;
    }

    return 0;
}

void mmc_init(int mmc_num) {
    mmc_clk_init(mmc_num);
    mmc_controller_init();
    card_init();
}