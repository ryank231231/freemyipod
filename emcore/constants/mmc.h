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


#ifndef __CONSTANTS_MMC_H__
#define __CONSTANTS_MMC_H__


#include "global.h"


#define MMC_CMD_GO_IDLE_STATE 0
#define MMC_CMD_SEND_OP_COND 1
#define MMC_CMD_ALL_SEND_CID 2
#define MMC_CMD_SET_RELATIVE_ADDR 3
#define MMC_CMD_SET_DSR 4
#define MMC_CMD_SLEEP_AWAKE 5
#define MMC_CMD_SWITCH 6
#define MMC_CMD_SELECT_CARD 7
#define MMC_CMD_SEND_EXT_CSD 8
#define MMC_CMD_SEND_CSD 9
#define MMC_CMD_SEND_CID 10
#define MMC_CMD_READ_DAT_UNTIL_STOP 11
#define MMC_CMD_STOP_TRANSMISSION 12
#define MMC_CMD_SEND_STATUS 13
#define MMC_CMD_BUSTEST_R 14
#define MMC_CMD_GO_INAVTIVE_STATE 15
#define MMC_CMD_SET_BLOCKLEN 16
#define MMC_CMD_READ_SINGLE_BLOCK 17
#define MMC_CMD_READ_MULTIPLE_BLOCK 18
#define MMC_CMD_BUSTEST_W 19
#define MMC_CMD_WRITE_DAT_UNTIL_STOP 20
#define MMC_CMD_SET_BLOCK_COUNT 23
#define MMC_CMD_WRITE_BLOCK 24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK 25
#define MMC_CMD_PROGRAM_CID 26
#define MMC_CMD_PROGRAM_CSD 27
#define MMC_CMD_SET_WRITE_PROT 28
#define MMC_CMD_CLR_WRITE_PROT 29
#define MMC_CMD_SEND_WRITE_PROT 30
#define MMC_CMD_ERASE_GROUP_START 35
#define MMC_CMD_ERASE_GROUP_END 36
#define MMC_CMD_ERASE 38
#define MMC_CMD_FAST_IO 39
#define MMC_CMD_GO_IRQ_STATE 40
#define MMC_CMD_LOCK_UNLOCK 42
#define MMC_CMD_APP_CMD 55
#define MMC_CMD_GEN_CMD 56
#define MMC_CMD_CEATA_RW_MULTIPLE_REG 60
#define MMC_CMD_CEATA_RW_MULTIPLE_BLOCK 61

#define MMC_CMD_SEND_OP_COND_OCR_MASK BITRANGE(0, 31)
#define MMC_CMD_SEND_OP_COND_OCR_SHIFT 0
#define MMC_CMD_SEND_OP_COND_OCR(x) (x)

#define MMC_CMD_SET_RELATIVE_ADDR_RCA_MASK BITRANGE(16, 31)
#define MMC_CMD_SET_RELATIVE_ADDR_RCA_SHIFT 16
#define MMC_CMD_SET_RELATIVE_ADDR_RCA(x) ((x) << 16)

#define MMC_CMD_SET_DSR_DSR_MASK BITRANGE(16, 31)
#define MMC_CMD_SET_DSR_DSR_SHIFT 16
#define MMC_CMD_SET_DSR_DSR(x) ((x) << 16)

#define MMC_CMD_SLEEP_AWAKE_SLEEP_AWAKE_MASK BIT(15)
#define MMC_CMD_SLEEP_AWAKE_SLEEP_AWAKE_AWAKE 0
#define MMC_CMD_SLEEP_AWAKE_SLEEP_AWAKE_SLEEP BIT(15)
#define MMC_CMD_SLEEP_AWAKE_RCA_MASK BITRANGE(16, 31)
#define MMC_CMD_SLEEP_AWAKE_RCA_SHIFT 16
#define MMC_CMD_SLEEP_AWAKE_RCA(x) ((x) << 16)

#define MMC_CMD_SWITCH_ACCESS_MASK BITRANGE(24, 25);
#define MMC_CMD_SWITCH_ACCESS_CMDSET 0
#define MMC_CMD_SWITCH_ACCESS_SET_BITS BIT(24)
#define MMC_CMD_SWITCH_ACCESS_CLEAR_BITS BIT(25)
#define MMC_CMD_SWITCH_ACCESS_WRITE_BYTE (BIT(24) | BIT(25))
#define MMC_CMD_SWTICH_INDEX_MASK BITRANGE(16, 23);
#define MMC_CMD_SWITCH_INDEX_SHIFT 16
#define MMC_CMD_SWITCH_INDEX(x) ((x) << 16)
#define MMC_CMD_SWTICH_VALUE_MASK BITRANGE(8, 15);
#define MMC_CMD_SWITCH_VALUE_SHIFT 8
#define MMC_CMD_SWITCH_VALUE(x) ((x) << 8)
#define MMC_CMD_SWTICH_CMDSET_MASK BITRANGE(0, 2);
#define MMC_CMD_SWITCH_CMDSET_STANDARD_MMC 0

#define MMC_CMD_SELECT_CARD_RCA_MASK BITRANGE(16, 31)
#define MMC_CMD_SELECT_CARD_RCA_SHIFT 16
#define MMC_CMD_SELECT_CARD_RCA(x) ((x) << 16)

#define MMC_CMD_SEND_CSD_RCA_MASK BITRANGE(16, 31)
#define MMC_CMD_SEND_CSD_RCA_SHIFT 16
#define MMC_CMD_SEND_CSD_RCA(x) ((x) << 16)

#define MMC_CMD_SEND_CID_RCA_MASK BITRANGE(16, 31)
#define MMC_CMD_SEND_CID_RCA_SHIFT 16
#define MMC_CMD_SEND_CID_RCA(x) ((x) << 16)

#define MMC_CMD_READ_DAT_UNTIL_STOP_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_READ_DAT_UNTIL_STOP_ADDRESS_SHIFT 0
#define MMC_CMD_READ_DAT_UNTIL_STOP_ADDRESS(x) (x)

#define MMC_CMD_SEND_STATUS_RCA_MASK BITRANGE(16, 31)
#define MMC_CMD_SEND_STATUS_RCA_SHIFT 16
#define MMC_CMD_SEND_STATUS_RCA(x) ((x) << 16)

#define MMC_CMD_GO_INACTIVE_STATE_RCA_MASK BITRANGE(16, 31)
#define MMC_CMD_GO_INACTIVE_STATE_RCA_SHIFT 16
#define MMC_CMD_GO_INACTIVE_STATE_RCA(x) ((x) << 16)

#define MMC_CMD_SET_BLOCKLEN_LENGTH_MASK BITRANGE(0, 31)
#define MMC_CMD_SET_BLOCKLEN_LENGTH_SHIFT 0
#define MMC_CMD_SET_BLOCKLEN_LENGTH(x) (x)

#define MMC_CMD_READ_SINGLE_BLOCK_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_READ_SINGLE_BLOCK_ADDRESS_SHIFT 0
#define MMC_CMD_READ_SINGLE_BLOCK_ADDRESS(x) (x)

#define MMC_CMD_READ_MULTIPLE_BLOCK_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_READ_MULTIPLE_BLOCK_ADDRESS_SHIFT 0
#define MMC_CMD_READ_MULTIPLE_BLOCK_ADDRESS(x) (x)

#define MMC_CMD_WRITE_DAT_UNTIL_STOP_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_WRITE_DAT_UNTIL_STOP_ADDRESS_SHIFT 0
#define MMC_CMD_WRITE_DAT_UNTIL_STOP_ADDRESS(x) (x)

#define MMC_CMD_SET_BLOCK_COUNT_RELIABLE BIT(31)
#define MMC_CMD_SET_BLOCK_COUNT_COUNT_MASK BITRANGE(0, 15)
#define MMC_CMD_SET_BLOCK_COUNT_COUNT_SHIFT 0
#define MMC_CMD_SET_BLOCK_COUNT_COUNT(x) (x)

#define MMC_CMD_WRITE_BLOCK_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_WRITE_BLOCK_ADDRESS_SHIFT 0
#define MMC_CMD_WRITE_BLOCK_ADDRESS(x) (x)

#define MMC_CMD_WRITE_MULTIPLE_BLOCK_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_WRITE_MULTIPLE_BLOCK_ADDRESS_SHIFT 0
#define MMC_CMD_WRITE_MULTIPLE_BLOCK_ADDRESS(x) (x)

#define MMC_CMD_SET_WRITE_PROT_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_SET_WRITE_PROT_ADDRESS_SHIFT 0
#define MMC_CMD_SET_WRITE_PROT_ADDRESS(x) (x)

#define MMC_CMD_CLR_WRITE_PROT_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_CLR_WRITE_PROT_ADDRESS_SHIFT 0
#define MMC_CMD_CLR_WRITE_PROT_ADDRESS(x) (x)

#define MMC_CMD_SEND_WRITE_PROT_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_SEND_WRITE_PROT_ADDRESS_SHIFT 0
#define MMC_CMD_SEND_WRITE_PROT_ADDRESS(x) (x)

#define MMC_CMD_ERASE_GROUP_START_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_ERASE_GROUP_START_ADDRESS_SHIFT 0
#define MMC_CMD_ERASE_GROUP_START_ADDRESS(x) (x)

#define MMC_CMD_ERASE_GROUP_END_ADDRESS_MASK BITRANGE(0, 31)
#define MMC_CMD_ERASE_GROUP_END_ADDRESS_SHIFT 0
#define MMC_CMD_ERASE_GROUP_END_ADDRESS(x) (x)

#define MMC_CMD_FAST_IO_RCA_MASK BITRANGE(16, 31)
#define MMC_CMD_FAST_IO_RCA_SHIFT 16
#define MMC_CMD_FAST_IO_RCA(x) ((x) << 16)
#define MMC_CMD_FAST_IO_DIRECTION_MASK BIT(15)
#define MMC_CMD_FAST_IO_DIRECTION_READ 0
#define MMC_CMD_FAST_IO_DIRECTION_WRITE BIT(15)
#define MMC_CMD_FAST_IO_ADDRESS_MASK BITRANGE(8, 14)
#define MMC_CMD_FAST_IO_ADDRESS_SHIFT 8
#define MMC_CMD_FAST_IO_ADDRESS(x) ((x) << 8)
#define MMC_CMD_FAST_IO_DATA_MASK BITRANGE(0, 7)
#define MMC_CMD_FAST_IO_DATA_SHIFT 0
#define MMC_CMD_FAST_IO_DATA(x) (x)

#define MMC_CMD_APP_CMD_RCA_MASK BITRANGE(16, 31)
#define MMC_CMD_APP_CMD_RCA_SHIFT 16
#define MMC_CMD_APP_CMD_RCA(x) ((x) << 16)

#define MMC_CMD_GEN_CMD_DIRECTION_MASK BIT(0)
#define MMC_CMD_GEN_CMD_DIRECTION_READ 0
#define MMC_CMD_GEN_CMD_DIRECTION_WRITE BIT(0)

#define MMC_CMD_CEATA_RW_MULTIPLE_REG_DIRECTION_MASK BIT(31)
#define MMC_CMD_CEATA_RW_MULTIPLE_REG_DIRECTION_READ 0
#define MMC_CMD_CEATA_RW_MULTIPLE_REG_DIRECTION_WRITE BIT(31)
#define MMC_CMD_CEATA_RW_MULTIPLE_REG_ADDRESS_MASK BITRANGE(16, 23)
#define MMC_CMD_CEATA_RW_MULTIPLE_REG_ADDRESS_SHIFT 16
#define MMC_CMD_CEATA_RW_MULTIPLE_REG_ADDRESS(x) ((x) << 16)
#define MMC_CMD_CEATA_RW_MULTIPLE_REG_COUNT_MASK BITRANGE(0, 7)
#define MMC_CMD_CEATA_RW_MULTIPLE_REG_COUNT_SHIFT 0
#define MMC_CMD_CEATA_RW_MULTIPLE_REG_COUNT(x) (x)

#define MMC_CMD_CEATA_RW_MULTIPLE_BLOCK_DIRECTION_MASK BIT(31)
#define MMC_CMD_CEATA_RW_MULTIPLE_BLOCK_DIRECTION_READ 0
#define MMC_CMD_CEATA_RW_MULTIPLE_BLOCK_DIRECTION_WRITE BIT(31)
#define MMC_CMD_CEATA_RW_MULTIPLE_BLOCK_COUNT_MASK BITRANGE(0, 15)
#define MMC_CMD_CEATA_RW_MULTIPLE_BLOCK_COUNT_SHIFT 0
#define MMC_CMD_CEATA_RW_MULTIPLE_BLOCK_COUNT(x) (x)

#define MMC_CMD_SWITCH_FIELD_ERASE_GROUP_DEF 175
#define MMC_CMD_SWITCH_FIELD_BOOT_BUS_WIDTH 177
#define MMC_CMD_SWITCH_FIELD_BOOT_CONFIG 179
#define MMC_CMD_SWITCH_FIELD_ERASED_MEM_CONT 181
#define MMC_CMD_SWITCH_FIELD_BUS_WIDTH 183
#define MMC_CMD_SWITCH_FIELD_HS_TIMING 185
#define MMC_CMD_SWITCH_FIELD_POWER_CLASS 187
#define MMC_CMD_SWITCH_FIELD_CMD_SET_REV 189
#define MMC_CMD_SWITCH_FIELD_CMD_SET 191
#define MMC_CMD_SWITCH_FIELD_EXT_CSD_REV 192
#define MMC_CMD_SWITCH_FIELD_CSD_STRUCTURE 194
#define MMC_CMD_SWITCH_FIELD_CARD_TYPE 196
#define MMC_CMD_SWITCH_FIELD_PWR_CL_52_195 200
#define MMC_CMD_SWITCH_FIELD_PWR_CL_26_195 201
#define MMC_CMD_SWITCH_FIELD_PWR_CL_52_360 202
#define MMC_CMD_SWITCH_FIELD_PWR_CL_26_360 203
#define MMC_CMD_SWITCH_FIELD_MIN_PERF_R_4_26 205
#define MMC_CMD_SWITCH_FIELD_MIN_PERF_W_4_26 206
#define MMC_CMD_SWITCH_FIELD_MIN_PERF_R_8_26_4_52 207
#define MMC_CMD_SWITCH_FIELD_MIN_PERF_W_8_26_4_52 208
#define MMC_CMD_SWITCH_FIELD_MIN_PERF_R_8_52 209
#define MMC_CMD_SWITCH_FIELD_MIN_PERF_W_8_52 210
#define MMC_CMD_SWITCH_FIELD_SEC_COUNT_0 212
#define MMC_CMD_SWITCH_FIELD_SEC_COUNT_1 213
#define MMC_CMD_SWITCH_FIELD_SEC_COUNT_2 214
#define MMC_CMD_SWITCH_FIELD_SEC_COUNT_3 215
#define MMC_CMD_SWITCH_FIELD_S_A_TIMEOUT 217
#define MMC_CMD_SWITCH_FIELD_S_C_VCCQ 219
#define MMC_CMD_SWITCH_FIELD_S_C_VCC 220
#define MMC_CMD_SWITCH_FIELD_HC_WP_GRP_SIZE 221
#define MMC_CMD_SWITCH_FIELD_REL_WR_SEC_C 222
#define MMC_CMD_SWITCH_FIELD_ERASE_TIMEOUT_MULT 223
#define MMC_CMD_SWITCH_FIELD_HC_ERASE_GRP_SIZE 224
#define MMC_CMD_SWITCH_FIELD_ACC_SIZE 225
#define MMC_CMD_SWITCH_FIELD_BOOT_SIZE_MULTI 226
#define MMC_CMD_SWITCH_FIELD_S_CMD_SET 504

#define MMC_CMD_SWITCH_FIELD_BUS_WIDTH_1BIT 0
#define MMC_CMD_SWITCH_FIELD_BUS_WIDTH_4BIT 1
#define MMC_CMD_SWITCH_FIELD_BUS_WIDTH_8BIT 2

#define MMC_CMD_SWITCH_FIELD_HS_TIMING_LOW_SPEED 0
#define MMC_CMD_SWITCH_FIELD_HS_TIMING_HIGH_SPEED 1

#define MMC_STATUS_APP_CMD BIT(5)
#define MMC_STATUS_SWITCH_ERROR BIT(7)
#define MMC_STATUS_READY_FOR_DATA BIT(8)
#define MMC_STATUS_CURRENT_STATE_MASK BITRANGE(9, 12)
#define MMC_STATUS_CURRENT_STATE_IDLE 0
#define MMC_STATUS_CURRENT_STATE_READY BIT(9)
#define MMC_STATUS_CURRENT_STATE_IDENT BIT(10)
#define MMC_STATUS_CURRENT_STATE_STBY (BIT(9) | BIT(10))
#define MMC_STATUS_CURRENT_STATE_TRAN BIT(11)
#define MMC_STATUS_CURRENT_STATE_DATA (BIT(9) | BIT(11))
#define MMC_STATUS_CURRENT_STATE_RCV (BIT(10) | BIT(11))
#define MMC_STATUS_CURRENT_STATE_PRG (BIT(9) | BIT(10) | BIT(11))
#define MMC_STATUS_CURRENT_STATE_DIS BIT(12)
#define MMC_STATUS_CURRENT_STATE_BTST (BIT(9) | BIT(12))
#define MMC_STATUS_CURRENT_STATE_SLP (BIT(10) | BIT(12))
#define MMC_STATUS_ERASE_RESET BIT(13)
#define MMC_STATUS_WP_ERASE_SKIP BIT(15)
#define MMC_STATUS_CID_CSD_OVERWRITE BIT(16)
#define MMC_STATUS_OVERRUN BIT(17)
#define MMC_STATUS_UNDERRUN BIT(18)
#define MMC_STATUS_ERROR BIT(19)
#define MMC_STATUS_CC_ERROR BIT(20)
#define MMC_STATUS_CARD_ECC_FAILED BIT(21)
#define MMC_STATUS_ILLEGAL_COMMAND BIT(22)
#define MMC_STATUS_COM_CRC_ERROR BIT(23)
#define MMC_STATUS_LOCK_UNLOCK_FAILED BIT(24)
#define MMC_STATUS_CARD_IS_LOCKED BIT(25)
#define MMC_STATUS_WP_VIOLATION BIT(26)
#define MMC_STATUS_ERASE_PARAM BIT(27)
#define MMC_STATUS_ERASE_SEQ_ERROR BIT(28)
#define MMC_STATUS_BLOCK_LEN_ERROR BIT(29)
#define MMC_STATUS_ADDRESS_MISALIGN BIT(30)
#define MMC_STATUS_ADDRESS_OUT_OF_RANGE BIT(31)

#define MMC_OCR_170_195 BIT(7)
#define MMC_OCR_200_260 BITRANGE(8, 14)
#define MMC_OCR_270_360 BITRANGE(15, 23)
#define MMC_OCR_ACCESS_MODE_MASK BITRANGE(29, 30)
#define MMC_OCR_ACCESS_MODE_BYTE 0
#define MMC_OCR_ACCESS_MODE_SECTOR BIT(30)
#define MMC_OCR_POWER_UP_DONE BIT(31)


#endif
