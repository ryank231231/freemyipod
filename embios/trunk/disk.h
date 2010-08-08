/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: disk.h 26629 2010-06-06 13:28:13Z gevaerts $
 *
 * Copyright (C) 2002 by Björn Stenberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#ifndef _DISK_H_
#define _DISK_H_

#include "mv.h" /* for volume definitions */

struct partinfo {
    unsigned long start; /* first sector (LBA) */
    unsigned long size;  /* number of sectors */
    unsigned char type;
};

#define PARTITION_TYPE_FAT32                0x0b
#define PARTITION_TYPE_FAT32_LBA            0x0c
#define PARTITION_TYPE_FAT16                0x06
#define PARTITION_TYPE_OS2_HIDDEN_C_DRIVE   0x84

/* returns a pointer to an array of 8 partinfo structs */
struct partinfo* disk_init(IF_MD_NONVOID(int drive));
struct partinfo* disk_partinfo(int partition);

void disk_init_subsystem(void); /* Initialises mutexes */
int disk_mount_all(void); /* returns the # of successful mounts */
int disk_mount(int drive);
int disk_unmount(int drive);

/* The number of 512-byte sectors in a "logical" sector. Needed for ipod 5.5G */
#ifdef MAX_LOG_SECTOR_SIZE
extern int disk_sector_multiplier;
#endif

#endif
