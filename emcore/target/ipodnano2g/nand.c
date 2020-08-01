//
//
//    Copyright 2010 TheSeven
//
//
//    This file is part of emCORE.
//
//    emCORE is free software: you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the
//    License, or (at your option) any later version.
//
//    emCORE is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with emCORE.  If not, see <http://www.gnu.org/licenses/>.
//
//


#include "global.h"
#include "panic.h"
#include "thread.h"
#include "util.h"
#include "timer.h"
#include "nand.h"
#include "pmu.h"
#include "mmu.h"
#include "s5l8701.h"
 
#define NAND_CMD_READ       0x00
#define NAND_CMD_PROGCNFRM  0x10
#define NAND_CMD_READ2      0x30
#define NAND_CMD_BLOCKERASE 0x60
#define NAND_CMD_GET_STATUS 0x70
#define NAND_CMD_PROGRAM    0x80
#define NAND_CMD_ERASECNFRM 0xD0
#define NAND_CMD_RESET      0xFF

#define NAND_STATUS_READY   0x40

static const struct nand_device_info_type nand_deviceinfotable[] =
{
    {0x1580F1EC, 1024, 968, 0x40, 6, 2, 1, 2, 1},
    {0x1580DAEC, 2048, 1936, 0x40, 6, 2, 1, 2, 1},
    {0x15C1DAEC, 2048, 1936, 0x40, 6, 2, 1, 2, 1},
    {0x1510DCEC, 4096, 3872, 0x40, 6, 2, 1, 2, 1},
    {0x95C1DCEC, 4096, 3872, 0x40, 6, 2, 1, 2, 1},
    {0x2514DCEC, 2048, 1936, 0x80, 7, 2, 1, 2, 1},
    {0x2514D3EC, 4096, 3872, 0x80, 7, 2, 1, 2, 1},
    {0x2555D3EC, 4096, 3872, 0x80, 7, 2, 1, 2, 1},
    {0x2555D5EC, 8192, 7744, 0x80, 7, 2, 1, 2, 1},
    {0x2585D3AD, 4096, 3872, 0x80, 7, 3, 2, 3, 2},
    {0x9580DCAD, 4096, 3872, 0x40, 6, 3, 2, 3, 2},
    {0xA514D3AD, 4096, 3872, 0x80, 7, 3, 2, 3, 2},
    {0xA550D3AD, 4096, 3872, 0x80, 7, 3, 2, 3, 2},
    {0xA560D5AD, 4096, 3872, 0x80, 7, 3, 2, 3, 2},
    {0xA555D5AD, 8192, 7744, 0x80, 7, 3, 2, 3, 2},
    {0xA585D598, 8320, 7744, 0x80, 7, 3, 1, 2, 1},
    {0xA584D398, 4160, 3872, 0x80, 7, 3, 1, 2, 1},
    {0x95D1D32C, 8192, 7744, 0x40, 6, 2, 1, 2, 1},
    {0x1580DC2C, 4096, 3872, 0x40, 6, 2, 1, 2, 1},
    {0x15C1D32C, 8192, 7744, 0x40, 6, 2, 1, 2, 1},
    {0x9590DC2C, 4096, 3872, 0x40, 6, 2, 1, 2, 1},
    {0xA594D32C, 4096, 3872, 0x80, 7, 2, 1, 2, 1},
    {0x2584DC2C, 2048, 1936, 0x80, 7, 2, 1, 2, 1},
    {0xA5D5D52C, 8192, 7744, 0x80, 7, 3, 2, 2, 1},
    {0x95D1D389, 8192, 7744, 0x40, 6, 2, 1, 2, 1},
    {0x1580DC89, 4096, 3872, 0x40, 6, 2, 1, 2, 1},
    {0x15C1D389, 8192, 7744, 0x40, 6, 2, 1, 2, 1},
    {0x9590DC89, 4096, 3872, 0x40, 6, 2, 1, 2, 1},
    {0xA594D389, 4096, 3872, 0x80, 7, 2, 1, 2, 1},
    {0x2584DC89, 2048, 1936, 0x80, 7, 2, 1, 2, 1},
    {0xA5D5D589, 8192, 7744, 0x80, 7, 2, 1, 2, 1},
    {0xA514D320, 4096, 3872, 0x80, 7, 2, 1, 2, 1},
    {0xA555D520, 8192, 3872, 0x80, 7, 2, 1, 2, 1}
};

static uint8_t nand_tunk1[4];
static uint8_t nand_twp[4];
static uint8_t nand_tunk2[4];
static uint8_t nand_tunk3[4];
static int nand_type[4];
static int nand_powered = 0;
static int nand_interleaved = 0;
static int nand_cached = 0;
static long nand_last_activity_value = -1;
static struct scheduler_thread nand_thread_handle;
static uint32_t nand_stack[0x80] STACK_ATTR;

static struct mutex nand_mtx;
static struct wakeup nand_wakeup;
static struct mutex ecc_mtx;
static struct wakeup ecc_wakeup;

static uint8_t nand_ctrl[0x200] CACHEALIGN_ATTR;
static uint8_t nand_spare[0x40] CACHEALIGN_ATTR;
static uint8_t nand_ecc[0x30] CACHEALIGN_ATTR;


static uint32_t nand_unlock(uint32_t rc)
{
    nand_last_activity_value = USEC_TIMER;
    mutex_unlock(&nand_mtx);
    return rc;
}

static uint32_t ecc_unlock(uint32_t rc)
{
    mutex_unlock(&ecc_mtx);
    return rc;
}

static uint32_t nand_timeout(long timeout)
{
    if (TIME_AFTER(USEC_TIMER, timeout)) return 1;
    else
    {
        yield();
        return 0;
    }
}

static uint32_t nand_wait_rbbdone(void)
{
    uint32_t timeout = USEC_TIMER + 20000;
    while (!(FMCSTAT & FMCSTAT_RBBDONE))
        if (nand_timeout(timeout)) return 1;
    FMCSTAT = FMCSTAT_RBBDONE;
    return 0;
}

static uint32_t nand_wait_cmddone(void)
{
    uint32_t timeout = USEC_TIMER + 20000;
    while (!(FMCSTAT & FMCSTAT_CMDDONE))
        if (nand_timeout(timeout)) return 1;
    FMCSTAT = FMCSTAT_CMDDONE;
    return 0;
}

static uint32_t nand_wait_addrdone(void)
{
    uint32_t timeout = USEC_TIMER + 20000;
    while (!(FMCSTAT & FMCSTAT_ADDRDONE))
        if (nand_timeout(timeout)) return 1;
    FMCSTAT = FMCSTAT_ADDRDONE;
    return 0;
}

static uint32_t nand_wait_chip_ready(uint32_t bank)
{
    uint32_t timeout = USEC_TIMER + 20000;
    while (!(FMCSTAT & (FMCSTAT_BANK0READY << bank)))
        if (nand_timeout(timeout)) return 1;
    FMCSTAT = (FMCSTAT_BANK0READY << bank);
    return 0;
}

static void nand_set_fmctrl0(uint32_t bank, uint32_t flags)
{
    FMCTRL0 = (nand_tunk1[bank] << 16) | (nand_twp[bank] << 12)
            | (1 << 11) | 1 | (1 << (bank + 1)) | flags;
}

static uint32_t nand_send_cmd(uint32_t cmd)
{
    FMCMD = cmd;
    return nand_wait_rbbdone();
}

static uint32_t nand_send_address(uint32_t page, uint32_t offset)
{
    FMANUM = 4;
    FMADDR0 = (page << 16) | offset;
    FMADDR1 = (page >> 16) & 0xFF;
    FMCTRL1 = FMCTRL1_DOTRANSADDR;
    return nand_wait_cmddone();
}

uint32_t nand_reset(uint32_t bank)
{
    nand_set_fmctrl0(bank, 0);
    if (nand_send_cmd(NAND_CMD_RESET)) return 1;
    if (nand_wait_chip_ready(bank)) return 1;
    FMCTRL1 = FMCTRL1_CLEARRFIFO | FMCTRL1_CLEARWFIFO;
    sleep(1000);
    return 0;
}

static uint32_t nand_wait_status_ready(uint32_t bank)
{
    uint32_t timeout = USEC_TIMER + 20000;
    nand_set_fmctrl0(bank, 0);
    if ((FMCSTAT & (FMCSTAT_BANK0READY << bank)))
        FMCSTAT = (FMCSTAT_BANK0READY << bank);
    FMCTRL1 = FMCTRL1_CLEARRFIFO;
    if (nand_send_cmd(NAND_CMD_GET_STATUS)) return 1;
    while (1)
    {
        if (nand_timeout(timeout)) return 1;
        FMDNUM = 0;
        FMCTRL1 = FMCTRL1_DOREADDATA;
        if (nand_wait_addrdone()) return 1;
        if ((FMFIFO & NAND_STATUS_READY)) break;
        FMCTRL1 = FMCTRL1_CLEARRFIFO;
    }
    FMCTRL1 = FMCTRL1_CLEARRFIFO;
    return nand_send_cmd(NAND_CMD_READ);
}

static void nand_transfer_data_start(uint32_t bank, uint32_t direction,
                                     void* buffer, uint32_t size)
{
    nand_set_fmctrl0(bank, FMCTRL0_ENABLEDMA);
    FMDNUM = size - 1;
    FMCTRL1 = FMCTRL1_DOREADDATA << direction;
    DMACON3 = (2 << DMACON_DEVICE_SHIFT)
            | (direction << DMACON_DIRECTION_SHIFT)
            | (2 << DMACON_DATA_SIZE_SHIFT)
            | (3 << DMACON_BURST_LEN_SHIFT);
    while ((DMAALLST & DMAALLST_CHAN3_MASK))
        DMACOM3 = DMACOM_CLEARBOTHDONE;
    DMABASE3 = buffer;
    DMATCNT3 = (size >> 4) - 1;
    if (direction) clean_dcache();
    else invalidate_dcache();
    DMACOM3 = 4;
}

static uint32_t nand_transfer_data_collect(uint32_t direction)
{
    uint32_t timeout = USEC_TIMER + 20000;
    while (DMAALLST & DMAALLST_DMABUSY3)
        if (nand_timeout(timeout)) return 1;
    if (nand_wait_addrdone()) return 1;
    if (!direction) FMCTRL1 = FMCTRL1_CLEARRFIFO | FMCTRL1_CLEARWFIFO;
    else FMCTRL1 = FMCTRL1_CLEARRFIFO;
    return 0;
}

static uint32_t nand_transfer_data(uint32_t bank, uint32_t direction,
                                   void* buffer, uint32_t size)
{
    nand_transfer_data_start(bank, direction, buffer, size);
    uint32_t rc = nand_transfer_data_collect(direction);
    return rc;
}

static void ecc_start(uint32_t size, void* databuffer, void* sparebuffer,
                      uint32_t type)
{
    mutex_lock(&ecc_mtx, TIMEOUT_BLOCK);
    ECC_INT_CLR = 1;
    SRCPND = (1 << IRQ_ECC);
    ECC_UNK1 = size;
    ECC_DATA_PTR = databuffer;
    ECC_SPARE_PTR = sparebuffer;
    invalidate_dcache();
    ECC_CTRL = type;
}

static uint32_t ecc_collect(void)
{
    uint32_t timeout = USEC_TIMER + 20000;
    while (!(SRCPND & (1 << IRQ_ECC)))
        if (nand_timeout(timeout)) return ecc_unlock(1);
    ECC_INT_CLR = 1;
    SRCPND = (1 << IRQ_ECC);
    return ecc_unlock(ECC_RESULT);
}

static uint32_t ecc_decode(uint32_t size, void* databuffer, void* sparebuffer)
{
    ecc_start(size, databuffer, sparebuffer, ECCCTRL_STARTDECODING);
    uint32_t rc = ecc_collect();
    return rc;
}

static uint32_t ecc_encode(uint32_t size, void* databuffer, void* sparebuffer)
{
    ecc_start(size, databuffer, sparebuffer, ECCCTRL_STARTENCODING);
    ecc_collect();
    return 0;
}

static uint32_t nand_check_empty(uint8_t* buffer)
{
    uint32_t i, count;
    count = 0;
    for (i = 0; i < 0x40; i++) if (buffer[i] != 0xFF) count++;
    if (count < 2) return 1;
    return 0;
}

static uint32_t nand_get_chip_type(uint32_t bank)
{
    mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
    uint32_t result;
    if (nand_reset(bank)) return nand_unlock(0xFFFFFFFE);
    if (nand_send_cmd(0x90)) return nand_unlock(0xFFFFFFFD);
    FMANUM = 0;
    FMADDR0 = 0;
    FMCTRL1 = FMCTRL1_DOTRANSADDR;
    if (nand_wait_cmddone()) return nand_unlock(0xFFFFFFFC);
    FMDNUM = 4;
    FMCTRL1 = FMCTRL1_DOREADDATA;
    if (nand_wait_addrdone()) return nand_unlock(0xFFFFFFFB);
    result = FMFIFO;
    FMCTRL1 = FMCTRL1_CLEARRFIFO;
    return nand_unlock(result);
}

void nand_set_active(void)
{
    nand_last_activity_value = USEC_TIMER;
}

long nand_last_activity(void)
{
    return nand_last_activity_value;
}

void nand_power_up(void)
{
    uint32_t i;
    mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
    nand_last_activity_value = USEC_TIMER;
    PWRCON(1) &= ~0x40;
    PWRCON(0) &= ~0x100000;
    PCON2 = 0x33333333;
    PDAT2 = 0;
    PCON3 = 0x11113333;
    PDAT3 = 0;
    PCON4 = 0x33333333;
    PDAT4 = 0;
    PCON5 = (PCON5 & ~0xF) | 3;
    PUNK5 = 1;
    pmu_ldo_set_voltage(4, 0x15);
    pmu_ldo_power_on(4);
    sleep(50000);
    nand_last_activity_value = USEC_TIMER;
    for (i = 0; i < 4; i++)
        if (nand_type[i] >= 0)
            if (nand_reset(i))
				panicf(PANIC_KILLTHREAD, "nand_power_up: nand_reset(bank=%d) failed.", (unsigned int)i);
    nand_powered = 1;
    nand_last_activity_value = USEC_TIMER;
    mutex_unlock(&nand_mtx);
}

void nand_power_down(void)
{
    if (!nand_powered) return;
    mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
    pmu_ldo_power_off(4);
    PCON2 = 0x11111111;
    PDAT2 = 0;
    PCON3 = 0x11111111;
    PDAT3 = 0;
    PCON4 = 0x11111111;
    PDAT4 = 0;
    PCON5 = (PCON5 & ~0xF) | 1;
    PUNK5 = 1;
    PWRCON(1) |= 0x40;
    PWRCON(0) |= 0x100000;
    nand_powered = 0;
    mutex_unlock(&nand_mtx);
}

uint32_t nand_read_page(uint32_t bank, uint32_t page, void* databuffer,
                        void* sparebuffer, uint32_t doecc,
                        uint32_t checkempty)
{
#ifdef NAND_TRACE
    DEBUGF("NAND: Read bank %d, page %d", bank, page);
#endif
    uint8_t* data = (uint8_t*)databuffer;
    uint8_t* spare = nand_spare;
    if (sparebuffer) spare = (uint8_t*)sparebuffer;
	if ((uint32_t)databuffer & (CACHEALIGN_SIZE - 1))
		panicf(PANIC_KILLTHREAD,
	           "nand_read_page: Misaligned data buffer at %08X (bank %lu, page %lu)",
			   (unsigned int)databuffer, bank, page);
	if ((uint32_t)sparebuffer & (CACHEALIGN_SIZE - 1))
		panicf(PANIC_KILLTHREAD,
	           "nand_read_page: Misaligned spare buffer at %08X (bank %lu, page %lu)",
			   (unsigned int)sparebuffer, bank, page);
    mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
    nand_last_activity_value = USEC_TIMER;
    if (!nand_powered) nand_power_up();
    uint32_t rc, eccresult;
    nand_set_fmctrl0(bank, FMCTRL0_ENABLEDMA);
    if (nand_send_cmd(NAND_CMD_READ)) return nand_unlock(1);
    if (nand_send_address(page, data ? 0 : 0x800))
        return nand_unlock(1);
    if (nand_send_cmd(NAND_CMD_READ2)) return nand_unlock(1);
    if (nand_wait_status_ready(bank)) return nand_unlock(1);
    if (data)
        if (nand_transfer_data(bank, 0, data, 0x800))
            return nand_unlock(1);
    rc = 0;
    if (!doecc)
    {
        if (sparebuffer)
        {
            if (nand_transfer_data(bank, 0, spare, 0x40))
                return nand_unlock(1);
            if (checkempty)
                rc = nand_check_empty((uint8_t*)sparebuffer) << 1;
        }
        return nand_unlock(rc);
    }
    if (nand_transfer_data(bank, 0, spare, 0x40)) return nand_unlock(1);
    if (databuffer)
    {
        memcpy(nand_ecc, &spare[0xC], 0x28);
        rc |= (ecc_decode(3, data, nand_ecc) & 0xF) << 4;
    }
    memset(nand_ctrl, 0xFF, 0x200);
    memcpy(nand_ctrl, spare, 0xC);
    memcpy(nand_ecc, &spare[0x34], 0xC);
    eccresult = ecc_decode(0, nand_ctrl, nand_ecc);
    rc |= (eccresult & 0xF) << 8;
    if (sparebuffer)
    {
        if (eccresult & 1) memset(sparebuffer, 0xFF, 0xC);
        else memcpy(sparebuffer, nand_ctrl, 0xC);
    }
    if (checkempty) rc |= nand_check_empty(spare) << 1;
#ifdef NAND_DEBUG
    if ((rc & 2) == 0)
    {
        if ((rc & 0x10) != 0)
            DEBUGF("NAND: ECC failed to correct bank %d page %d user data!", bank, page);
        if ((rc & 0xE0) != 0)
            DEBUGF("NAND: ECC corrected %d errors in bank %d page %d user data!", rc >> 5, bank, page);
        if ((rc & 0x100) != 0)
            DEBUGF("NAND: ECC failed to correct bank %d page %d control data!", bank, page);
        if ((rc & 0xE00) != 0)
            DEBUGF("NAND: ECC corrected %d errors in bank %d page %d control data!", (rc >> 9) & 7, bank, page);
    }
#ifdef NAND_TRACE
    else DEBUGF("NAND: Bank %d page %d: Erased page!", bank, page);
#endif
#endif

#ifdef NAND_TRACE
    DEBUGF("NAND: Read success, RC=%X", rc);
#endif
    return nand_unlock(rc);
}

static uint32_t nand_write_page_int(uint32_t bank, uint32_t page,
                                    void* databuffer, void* sparebuffer,
                                    uint32_t doecc, uint32_t wait)
{
#ifdef NAND_TRACE
    DEBUGF("NAND: Write bank %d, page %d", bank, page);
#endif
    uint8_t* data = (uint8_t*)databuffer;
    uint8_t* spare = nand_spare;
    if (sparebuffer) spare = (uint8_t*)sparebuffer;
	if ((uint32_t)databuffer & 0xf)
		panicf(PANIC_KILLTHREAD,
	           "nand_write_page: Misaligned data buffer at %08X (bank %lu, page %lu)",
			   (unsigned int)databuffer, bank, page);
	if ((uint32_t)sparebuffer & 0xf)
		panicf(PANIC_KILLTHREAD,
	           "nand_write_page: Misaligned spare buffer at %08X (bank %lu, page %lu)",
			   (unsigned int)sparebuffer, bank, page);
    mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
    nand_last_activity_value = USEC_TIMER;
    if (!nand_powered) nand_power_up();
    if (!sparebuffer) memset(spare, 0xFF, 0x40);
    nand_set_fmctrl0(bank, FMCTRL0_ENABLEDMA);
    if (nand_send_cmd(NAND_CMD_PROGRAM)) return nand_unlock(1);
    if (nand_send_address(page, data ? 0 : 0x800))
        return nand_unlock(1);
    if (data) nand_transfer_data_start(bank, 1, data, 0x800);
    if (doecc)
    {
        if (ecc_encode(3, data, nand_ecc)) return nand_unlock(1);
        memcpy(&spare[0xC], nand_ecc, 0x28);
        memset(nand_ctrl, 0xFF, 0x200);
        memcpy(nand_ctrl, spare, 0xC);
        if (ecc_encode(0, nand_ctrl, nand_ecc)) return nand_unlock(1);
        memcpy(&spare[0x34], nand_ecc, 0xC);
    }
    if (data)
        if (nand_transfer_data_collect(1))
            return nand_unlock(1);
    if (sparebuffer || doecc)
        if (nand_transfer_data(bank, 1, spare, 0x40))
            return nand_unlock(1);
    if (nand_send_cmd(NAND_CMD_PROGCNFRM)) return nand_unlock(1);
    if (wait) if (nand_wait_status_ready(bank)) return nand_unlock(1);
    return nand_unlock(0);
}

uint32_t nand_block_erase(uint32_t bank, uint32_t page)
{
#ifdef NAND_TRACE
    DEBUGF("NAND: Block erase starting at bank %d, page %d", bank, page);
#endif
    mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
    nand_last_activity_value = USEC_TIMER;
    if (!nand_powered) nand_power_up();
    nand_set_fmctrl0(bank, 0);
    if (nand_send_cmd(NAND_CMD_BLOCKERASE)) return nand_unlock(1);
    FMANUM = 2;
    FMADDR0 = page;
    FMCTRL1 = FMCTRL1_DOTRANSADDR;
    if (nand_wait_cmddone()) return nand_unlock(1);
    if (nand_send_cmd(NAND_CMD_ERASECNFRM)) return nand_unlock(1);
    if (nand_wait_status_ready(bank)) return nand_unlock(1);
    return nand_unlock(0);
}

uint32_t nand_read_page_fast(uint32_t page, void* databuffer,
                             void* sparebuffer, uint32_t doecc,
                             uint32_t checkempty)
{
#ifdef NAND_TRACE
    DEBUGF("NAND: Read all banks, page %d", page);
#endif
    uint32_t i, rc = 0;
    if (((uint32_t)databuffer & 0xf) || ((uint32_t)sparebuffer & 0xf)
     || !databuffer || !sparebuffer || !doecc)
    {
        for (i = 0; i < 4; i++)
        {
            if (nand_type[i] < 0) continue;
            void* databuf = (void*)0;
            void* sparebuf = (void*)0;
            if (databuffer) databuf = (void*)((uint32_t)databuffer + 0x800 * i);
            if (sparebuffer) sparebuf = (void*)((uint32_t)sparebuffer + 0x40 * i);
            uint32_t ret = nand_read_page(i, page, databuf, sparebuf, doecc, checkempty);
            if (ret & 1) rc |= 1 << (i << 2);
            if (ret & 2) rc |= 2 << (i << 2);
            if (ret & 0x10) rc |= 4 << (i << 2);
            if (ret & 0x100) rc |= 8 << (i << 2);
        }
        return rc;
    }
    mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
    nand_last_activity_value = USEC_TIMER;
    if (!nand_powered) nand_power_up();
    uint8_t status[4];
    for (i = 0; i < 4; i++) status[i] = (nand_type[i] < 0);
    for (i = 0; i < 4; i++)
    {
        if (!status[i])
        {
            nand_set_fmctrl0(i, FMCTRL0_ENABLEDMA);
            if (nand_send_cmd(NAND_CMD_READ))
                status[i] = 1;
        }
        if (!status[i])
            if (nand_send_address(page, 0))
                status[i] = 1;
        if (!status[i])
            if (nand_send_cmd(NAND_CMD_READ2))
                status[i] = 1;
    }
    if (!status[0])
        if (nand_wait_status_ready(0))
            status[0] = 1;
    if (!status[0])
        if (nand_transfer_data(0, 0, databuffer, 0x800))
            status[0] = 1;
    if (!status[0])
        if (nand_transfer_data(0, 0, sparebuffer, 0x40))
            status[0] = 1;
    for (i = 1; i < 4; i++)
    {
        if (!status[i])
            if (nand_wait_status_ready(i))
                status[i] = 1;
        if (!status[i])
            nand_transfer_data_start(i, 0, (void*)((uint32_t)databuffer
                                                 + 0x800 * i), 0x800);
        if (!status[i - 1])
        {
            memcpy(nand_ecc, (void*)((uint32_t)sparebuffer + 0x40 * (i - 1) + 0xC), 0x28);
            ecc_start(3, (void*)((uint32_t)databuffer
                               + 0x800 * (i - 1)), nand_ecc, ECCCTRL_STARTDECODING);
        }
        if (!status[i])
            if (nand_transfer_data_collect(0))
                status[i] = 1;
        if (!status[i])
            nand_transfer_data_start(i, 0, (void*)((uint32_t)sparebuffer
                                                 + 0x40 * i), 0x40);
        if (!status[i - 1])
            if (ecc_collect() & 1)
                status[i - 1] = 4;
        if (!status[i - 1])
        {
            memset(nand_ctrl, 0xFF, 0x200);
            memcpy(nand_ctrl, (void*)((uint32_t)sparebuffer + 0x40 * (i - 1)), 0xC);
            memcpy(nand_ecc, (void*)((uint32_t)sparebuffer + 0x40 * (i - 1) + 0x34), 0xC);
            ecc_start(0, nand_ctrl, nand_ecc, ECCCTRL_STARTDECODING);
        }
        if (!status[i])
            if (nand_transfer_data_collect(0))
                status[i] = 1;
        if (!status[i - 1])
        {
            if (ecc_collect() & 1)
            {
                status[i - 1] |= 8;
                memset((void*)((uint32_t)sparebuffer + 0x40 * (i - 1)), 0xFF, 0xC);
            }
            else memcpy((void*)((uint32_t)sparebuffer + 0x40 * (i - 1)), nand_ctrl, 0xC);
            if (checkempty)
                status[i - 1] |= nand_check_empty((void*)((uint32_t)sparebuffer
                                                        + 0x40 * (i - 1))) << 1;
        }
    }
    if (!status[i - 1])
    {
        memcpy(nand_ecc,(void*)((uint32_t)sparebuffer + 0x40 * (i - 1) + 0xC), 0x28);
        if (ecc_decode(3, (void*)((uint32_t)databuffer
                                + 0x800 * (i - 1)), nand_ecc) & 1)
            status[i - 1] = 4;
    }
    if (!status[i - 1])
    {
        memset(nand_ctrl, 0xFF, 0x200);
        memcpy(nand_ctrl, (void*)((uint32_t)sparebuffer + 0x40 * (i - 1)), 0xC);
        memcpy(nand_ecc, (void*)((uint32_t)sparebuffer + 0x40 * (i - 1) + 0x34), 0xC);
        if (ecc_decode(0, nand_ctrl, nand_ecc) & 1)
        {
            status[i - 1] |= 8;
            memset((void*)((uint32_t)sparebuffer + 0x40 * (i - 1)), 0xFF, 0xC);
        }
        else memcpy((void*)((uint32_t)sparebuffer + 0x40 * (i - 1)), nand_ctrl, 0xC);
        if (checkempty)
            status[i - 1] |= nand_check_empty((void*)((uint32_t)sparebuffer
                                                    + 0x40 * (i - 1))) << 1;
    }
    for (i = 0; i < 4; i++)
        if (nand_type[i] < 0)
            rc |= status[i] << (i << 2);
    return nand_unlock(rc);
}

uint32_t nand_write_page(uint32_t bank, uint32_t page, void* databuffer,
                         void* sparebuffer, uint32_t doecc)
{
    return nand_write_page_int(bank, page, databuffer, sparebuffer, doecc, 1);
}

uint32_t nand_write_page_start(uint32_t bank, uint32_t page, void* databuffer,
                               void* sparebuffer, uint32_t doecc)
{
#ifdef NAND_TRACE
    DEBUGF("NAND: Write all banks, page %d", page);
#endif
    if (((uint32_t)databuffer & 0xf) || ((uint32_t)sparebuffer & 0xf)
     || !databuffer || !sparebuffer || !doecc || !nand_interleaved)
        return nand_write_page_int(bank, page, databuffer, sparebuffer, doecc, !nand_interleaved);

    mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
    nand_last_activity_value = USEC_TIMER;
    if (!nand_powered) nand_power_up();
    nand_set_fmctrl0(bank, FMCTRL0_ENABLEDMA);
    if (nand_send_cmd(NAND_CMD_PROGRAM))
        return nand_unlock(1);
    if (nand_send_address(page, 0))
        return nand_unlock(1);
    nand_transfer_data_start(bank, 1, databuffer, 0x800);
    if (ecc_encode(3, databuffer, nand_ecc))
        return nand_unlock(1);
    memcpy((void*)((uint32_t)sparebuffer + 0xC), nand_ecc, 0x28);
    memset(nand_ctrl, 0xFF, 0x200);
    memcpy(nand_ctrl, sparebuffer, 0xC);
    if (ecc_encode(0, nand_ctrl, nand_ecc))
        return nand_unlock(1);
    memcpy((void*)((uint32_t)sparebuffer + 0x34), nand_ecc, 0xC);
    if (nand_transfer_data_collect(0))
        return nand_unlock(1);
    if (nand_transfer_data(bank, 1, sparebuffer, 0x40))
        return nand_unlock(1);
    return nand_unlock(nand_send_cmd(NAND_CMD_PROGCNFRM));
}

uint32_t nand_write_page_collect(uint32_t bank)
{
    return nand_wait_status_ready(bank);
}

static uint32_t nand_block_erase_fast(uint32_t page)
{
    uint32_t i, rc = 0;
    mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
    nand_last_activity_value = USEC_TIMER;
    if (!nand_powered) nand_power_up();
    for (i = 0; i < 4; i++)
    {
        if (nand_type[i] < 0) continue;
        nand_set_fmctrl0(i, 0);
        if (nand_send_cmd(NAND_CMD_BLOCKERASE))
        {
            rc |= 1 << i;
            continue;
        }
        FMANUM = 2;
        FMADDR0 = page;
        FMCTRL1 = FMCTRL1_DOTRANSADDR;
        if (nand_wait_cmddone())
        {
            rc |= 1 << i;
            continue;
        }
        if (nand_send_cmd(NAND_CMD_ERASECNFRM)) rc |= 1 << i;
    }
    for (i = 0; i < 4; i++)
    {
        if (nand_type[i] < 0) continue;
        if (rc & (1 << i)) continue;
        if (nand_wait_status_ready(i)) rc |= 1 << i;
    }
    return nand_unlock(rc);
}

const struct nand_device_info_type* nand_get_device_type(uint32_t bank)
{
    if (nand_type[bank] < 0)
        return (struct nand_device_info_type*)0;
    return &nand_deviceinfotable[nand_type[bank]];
}

static void nand_thread(void* arg0, void* arg1, void* arg2, void* arg3)
{
    while (1)
    {
        mutex_lock(&nand_mtx, TIMEOUT_BLOCK);
        if (TIME_AFTER(USEC_TIMER, nand_last_activity_value + 200000) && nand_powered)
            nand_power_down();
        mutex_unlock(&nand_mtx);
        sleep(100000);
    }
}

int nand_device_init(void)
{
    mutex_init(&nand_mtx);
    wakeup_init(&nand_wakeup);
    mutex_init(&ecc_mtx);
    wakeup_init(&ecc_wakeup);

    uint32_t type;
    uint32_t i, j;

    /* Assume there are 0 banks, to prevent
       nand_power_up from talking with them yet. */
    for (i = 0; i < 4; i++) nand_type[i] = -1;
    nand_power_up();

    /* Now that the flash is powered on, detect how
       many banks we really have and initialize them. */
    for (i = 0; i < 4; i++)
    {
        nand_tunk1[i] = 7;
        nand_twp[i] = 7;
        nand_tunk2[i] = 7;
        nand_tunk3[i] = 7;
        type = nand_get_chip_type(i);
        if (type >= 0xFFFFFFF0)
        {
            nand_type[i] = (int)type;
            continue;
        }
        for (j = 0; ; j++)
        {
            if (j == ARRAYLEN(nand_deviceinfotable)) break;
            else if (nand_deviceinfotable[j].id == type)
            {
                nand_type[i] = j;
                break;
            }
        }
        nand_tunk1[i] = nand_deviceinfotable[nand_type[i]].tunk1;
        nand_twp[i] = nand_deviceinfotable[nand_type[i]].twp;
        nand_tunk2[i] = nand_deviceinfotable[nand_type[i]].tunk2;
        nand_tunk3[i] = nand_deviceinfotable[nand_type[i]].tunk3;
    }
    if (nand_type[0] < 0) return nand_type[0];
    nand_interleaved = ((nand_type[0] >> 22) & 1);
    nand_cached = ((nand_type[0] >> 23) & 1);

    nand_last_activity_value = USEC_TIMER;
    thread_create(&nand_thread_handle, "NAND idle monitor", nand_thread, nand_stack,
                  sizeof(nand_stack), OS_THREAD, 1, true, NULL, NULL, NULL, NULL);

    return 0;
}
