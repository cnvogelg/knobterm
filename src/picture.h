/*
 * picture.h - read picture from sd card
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
 *
 * This file is part of knobterm.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#ifndef PICTURE_H
#define PICTURE_H

#include "global.h"

#define PIC_MOUNT_ERROR   0x10
#define PIC_OPEN_ERROR    0x11
#define PIC_READ_ERROR    0x12
#define PIC_HEADER_ERROR  0x13
#define PIC_WIDTH_ERROR   0x14
#define PIC_HEIGHT_ERROR  0x15
#define PIC_HEADER_TOO_LONG 0x16

extern u08 picture_load(const char *name, u08 x, u08 y);

#endif