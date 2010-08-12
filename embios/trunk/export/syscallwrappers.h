//
//
//    Copyright 2010 TheSeven
//
//
//    This file is part of emBIOS.
//
//    emBIOS is free software: you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the
//    License, or (at your option) any later version.
//
//    emBIOS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with emBIOS.  If not, see <http://www.gnu.org/licenses/>.
//
//


#ifndef __SYSCALLWRAPPERS_H__
#define __SYSCALLWRAPPERS_H__


#define IN_APPLICATION_CODE
#include "syscallapi.h"


extern struct embios_syscall_table* __embios_syscall;


#define panic(args...) __embios_syscall->panic(args)
#define panicf(args...) __embios_syscall->panicf(args)
#define cprintf(args...) __embios_syscall->cprintf(args)
#define cvprintf(args...) __embios_syscall->cvprintf(args)
#define cputc(args...) __embios_syscall->cputc(args)
#define cputs(args...) __embios_syscall->cputs(args)
#define cwrite(args...) __embios_syscall->cwrite(args)
#define cflush(args...) __embios_syscall->cflush(args)
#define cgetc(args...) __embios_syscall->cgetc(args)
#define cread(args...) __embios_syscall->cread(args)
#define creada(args...) __embios_syscall->creada(args)
#define opendir(args...) __embios_syscall->opendir(args)
#define closedir(args...) __embios_syscall->closedir(args)
#define readdir(args...) __embios_syscall->readdir(args)
#define mkdir(args...) __embios_syscall->mkdir(args)
#define rmdir(args...) __embios_syscall->rmdir(args)
#define renderbmp(args...) __embios_syscall->renderbmp(args)
#define renderchar(args...) __embios_syscall->renderchar(args)
#define execimage(args...) __embios_syscall->execimage(args)
#define ftruncate(args...) __embios_syscall->ftruncate(args)
#define fsync(args...) __embios_syscall->fsync(args)
#define close(args...) __embios_syscall->close(args)
#define write(args...) __embios_syscall->write(args)
#define read(args...) __embios_syscall->read(args)
#define lseek(args...) __embios_syscall->lseek(args)
#define remove(args...) __embios_syscall->remove(args)
#define file_open(args...) __embios_syscall->file_open(args)
#define rename(args...) __embios_syscall->rename(args)
#define file_creat(args...) __embios_syscall->file_creat(args)
#define filesize(args...) __embios_syscall->filesize(args)
#define format(args...) __embios_syscall->format(args)
#define vuprintf(args...) __embios_syscall->vuprintf(args)
#define lcdconsole_putc_noblit(args...) __embios_syscall->lcdconsole_putc_noblit(args)
#define lcdconsole_puts_noblit(args...) __embios_syscall->lcdconsole_puts_noblit(args)
#define lcdconsole_write_noblit(args...) __embios_syscall->lcdconsole_write_noblit(args)
#define lcdconsole_update(args...) __embios_syscall->lcdconsole_update(args)
#define lcdconsole_putc(args...) __embios_syscall->lcdconsole_putc(args)
#define lcdconsole_puts(args...) __embios_syscall->lcdconsole_puts(args)
#define lcdconsole_write(args...) __embios_syscall->lcdconsole_write(args)
#define shutdown(args...) __embios_syscall->shutdown(args)
#define storage_read_sectors(args...) __embios_syscall->storage_read_sectors(args)
#define storage_write_sectors(args...) __embios_syscall->storage_write_sectors(args)
#define strcasecmp(args...) __embios_syscall->strcasecmp(args)
#define strncasecmp(args...) __embios_syscall->strncasecmp(args)
#define strcasestr(args...) __embios_syscall->strcasestr(args)
#define strlcat(args...) __embios_syscall->strlcat(args)
#define strlcpy(args...) __embios_syscall->strlcpy(args)
#define mutex_init(args...) __embios_syscall->mutex_init(args)
#define mutex_lock(args...) __embios_syscall->mutex_lock(args)
#define mutex_unlock(args...) __embios_syscall->mutex_unlock(args)
#define wakeup_init(args...) __embios_syscall->wakeup_init(args)
#define wakeup_wait(args...) __embios_syscall->wakeup_wait(args)
#define wakeup_signal(args...) __embios_syscall->wakeup_signal(args)
#define sleep(args...) __embios_syscall->sleep(args)
#define thread_create(args...) __embios_syscall->thread_create(args)
#define thread_exit(args...) __embios_syscall->thread_exit(args)
#define thread_suspend(args...) __embios_syscall->thread_suspend(args)
#define thread_resume(args...) __embios_syscall->thread_resume(args)
#define thread_terminate(args...) __embios_syscall->thread_terminate(args)
#define __errno(args...) __embios_syscall->__errno(args)
#define ucl_decompress(args...) __embios_syscall->ucl_decompress(args)
#define bootflash_filesize(args...) __embios_syscall->bootflash_filesize(args)
#define bootflash_attributes(args...) __embios_syscall->bootflash_attributes(args)
#define bootflash_getaddr(args...) __embios_syscall->bootflash_getaddr(args)
#define bootflash_read(args...) __embios_syscall->bootflash_read(args)
#define bootflash_readraw(args...) __embios_syscall->bootflash_readraw(args)
#define bootflash_writeraw(args...) __embios_syscall->bootflash_writeraw(args)
#define bootflash_getrawaddr(args...) __embios_syscall->bootflash_getrawaddr(args)
#define read_native_timer(args...) __embios_syscall->read_native_timer(args)
#define read_usec_timer(args...) __embios_syscall->read_usec_timer(args)
#define i2c_send(args...) __embios_syscall->i2c_send(args)
#define i2c_recv(args...) __embios_syscall->i2c_recv(args)
#define i2c_sendbyte(args...) __embios_syscall->i2c_sendbyte(args)
#define i2c_recvbyte(args...) __embios_syscall->i2c_recvbyte(args)
#define interrupt_enable(args...) __embios_syscall->interrupt_enable(args)
#define interrupt_set_handler(args...) __embios_syscall->interrupt_set_handler(args)
#define int_timer_set_handler(args...) __embios_syscall->int_timer_set_handler(args)
#define displaylcd(args...) __embios_syscall->displaylcd(args)
#define displaylcd_sync(args...) __embios_syscall->displaylcd_sync(args)
#define displaylcd_busy(args...) __embios_syscall->displaylcd_busy(args)
#define displaylcd_safe(args...) __embios_syscall->displaylcd_safe(args)
#define clean_dcache(args...) __embios_syscall->clean_dcache(args)
#define invalidate_dcache(args...) __embios_syscall->invalidate_dcache(args)
#define invalidate_icache(args...) __embios_syscall->invalidate_icache(args)
#define nand_read_page(args...) __embios_syscall->nand_read_page(args)
#define nand_block_erase(args...) __embios_syscall->nand_block_erase(args)
#define nand_read_page_fast(args...) __embios_syscall->nand_read_page_fast(args)
#define nand_write_page(args...) __embios_syscall->nand_write_page(args)
#define nand_write_page_start(args...) __embios_syscall->nand_write_page_start(args)
#define nand_write_page_collect(args...) __embios_syscall->nand_write_page_collect(args)
#define nand_get_device_type(args...) __embios_syscall->nand_get_device_type(args)
#define power_off(args...) __embios_syscall->power_off(args)
#define charging_state(args...) __embios_syscall->charging_state(args)
#define atoi(args...) __embios_syscall->atoi(args)
#define memchr(args...) __embios_syscall->memchr(args)
#define memcmp(args...) __embios_syscall->memcmp(args)
#define memcpy(args...) __embios_syscall->memcpy(args)
#define memmove(args...) __embios_syscall->memmove(args)
#define memset(args...) __embios_syscall->memset(args)
#define qsort(args...) __embios_syscall->qsort(args)
#define srand(args...) __embios_syscall->srand(args)
#define rand(args...) __embios_syscall->rand(args)
#define snprintf(args...) __embios_syscall->snprintf(args)
#define vsnprintf(args...) __embios_syscall->vsnprintf(args)
#define isspace(args...) __embios_syscall->isspace(args)
#define isdigit(args...) __embios_syscall->isdigit(args)
#define isxdigit(args...) __embios_syscall->isxdigit(args)
#define sscanf(args...) __embios_syscall->sscanf(args)
#define strcat(args...) __embios_syscall->strcat(args)
#define strchr(args...) __embios_syscall->strchr(args)
#define strcmp(args...) __embios_syscall->strcmp(args)
#define strcpy(args...) __embios_syscall->strcpy(args)
#define strlen(args...) __embios_syscall->strlen(args)
#define strncmp(args...) __embios_syscall->strncmp(args)
#define strrchr(args...) __embios_syscall->strrchr(args)
#define strstr(args...) __embios_syscall->strstr(args)
#define strtok_r(args...) __embios_syscall->strtok_r(args)


#endif
