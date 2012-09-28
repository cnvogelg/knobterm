/*
 * screen.h - text and image drawing
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

#ifndef SCREEN_H
#define SCREEN_H

#define FGBG(x,y) ((x)<<4 | (y))

#define FLAGS_FONT_2X    0x01
#define FLAGS_FONT_2Y    0x02
#define FLAGS_FONT_2XY   (FLAGS_FONT_2X | FLAGS_FONT_2Y)

extern void screen_init(void);
extern void screen_clear(u08 col);
extern void screen_erase(u08 x, u08 y, u08 w, u08 h, u08 col);
extern void screen_putch(u08 x, u08 y, u08 ch, u08 col, u08 flags);
extern void screen_puts(u08 x, u08 y, const char *str, u08 col, u08 flags);

#endif
