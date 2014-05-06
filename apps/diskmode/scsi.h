//
//
//    Copyright 2013 TheSeven
//    Copyright 2014 user890104
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


#ifndef __SCSI_H__
#define __SCSI_H__

#include "emcoreapp.h"


#define SCSI_TEST_UNIT_READY        0x00
#define SCSI_INQUIRY                0x12
#define SCSI_MODE_SENSE_6           0x1a
#define SCSI_MODE_SENSE_10          0x5a
#define SCSI_REQUEST_SENSE          0x03
#define SCSI_ALLOW_MEDIUM_REMOVAL   0x1e
#define SCSI_READ_CAPACITY          0x25
#define SCSI_READ_FORMAT_CAPACITY   0x23
#define SCSI_READ_10                0x28
#define SCSI_WRITE_10               0x2a
#define SCSI_START_STOP_UNIT        0x1b
#define SCSI_REPORT_LUNS            0xa0
#define SCSI_WRITE_BUFFER           0x3b

#define SENSE_NOT_READY             0x02
#define SENSE_MEDIUM_ERROR          0x03
#define SENSE_ILLEGAL_REQUEST       0x05
#define SENSE_UNIT_ATTENTION        0x06

#define ASC_MEDIUM_NOT_PRESENT      0x3a
#define ASC_INVALID_FIELD_IN_CBD    0x24
#define ASC_LBA_OUT_OF_RANGE        0x21
#define ASC_WRITE_ERROR             0x0C
#define ASC_READ_ERROR              0x11
#define ASC_NOT_READY               0x04
#define ASC_INVALID_COMMAND         0x20

#define ASCQ_BECOMING_READY         0x01

#define DIRECT_ACCESS_DEVICE        0x00
#define DEVICE_REMOVABLE            0x80

#define SCSI_FORMAT_CAPACITY_FORMATTED_MEDIA 0x02000000


struct __attribute__((packed)) inquiry_data
{
    unsigned char DeviceType;
    unsigned char DeviceTypeModifier;
    unsigned char Versions;
    unsigned char Format;
    unsigned char AdditionalLength;
    unsigned char Reserved[2];
    unsigned char Capability;
    char VendorId[8];
    char ProductId[16];
    char ProductRevisionLevel[4];
};

struct __attribute__((packed)) report_lun_data
{
    unsigned int lun_list_length;
    unsigned int reserved1;
    unsigned char luns[1][8];
};

struct __attribute__((packed)) sense_data
{
    unsigned char ResponseCode;
    unsigned char Obsolete;
    unsigned char fei_sensekey;
    unsigned int Information;
    unsigned char AdditionalSenseLength;
    unsigned int  CommandSpecificInformation;
    unsigned char AdditionalSenseCode;
    unsigned char AdditionalSenseCodeQualifier;
    unsigned char FieldReplaceableUnitCode;
    unsigned char SKSV;
    unsigned short SenseKeySpecific;
};

struct __attribute__((packed)) mode_sense_bdesc_longlba
{
    unsigned char num_blocks[8];
    unsigned char reserved[4];
    unsigned char block_size[4];
};

struct __attribute__((packed)) mode_sense_bdesc_shortlba
{
    unsigned char density_code;
    unsigned char num_blocks[3];
    unsigned char reserved;
    unsigned char block_size[3];
};

struct __attribute__((packed)) mode_sense_data_10
{
    unsigned short mode_data_length;
    unsigned char medium_type;
    unsigned char device_specific;
    unsigned char longlba;
    unsigned char reserved;
    unsigned short block_descriptor_length;
    struct mode_sense_bdesc_longlba block_descriptor;
};

struct __attribute__((packed)) mode_sense_data_6
{
    unsigned char mode_data_length;
    unsigned char medium_type;
    unsigned char device_specific;
    unsigned char block_descriptor_length;
    struct mode_sense_bdesc_shortlba block_descriptor;
};

struct __attribute__((packed)) capacity
{
    unsigned int block_count;
    unsigned int block_size;
};

struct __attribute__((packed)) format_capacity
{
    unsigned int following_length;
    unsigned int block_count;
    unsigned int block_size;
};


#endif

