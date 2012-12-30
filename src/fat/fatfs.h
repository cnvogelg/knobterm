/*
 * fatfs.h
 *
 * small adapter to Chan FatFS
 */

#ifndef FATFS_H
#define FATFS_H

#include "integer.h"
#include "ff.h"
#include "mmc.h"

extern FATFS fatfs;

extern void fatfs_init(uint8_t clock_div);

#define fatfs_service disk_timerproc

extern uint8_t fatfs_mount(void);
extern void fatfs_umount(void);
extern uint8_t fatfs_is_mounted(void);

#endif
