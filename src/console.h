/*
 * console.h - console output
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

#ifndef CONSOLE_H
#define CONSOLE_H

typedef struct {
  u08 width;
  u08 height;
  u08 top_x;
  u08 top_y;
  u08 cursor_x;
  u08 cursor_y;
  u08 *buffer;
  u08 color;
  u08 flags;
  u08 cursor_color; /* 0 = disable cursor */
} console_t;

extern void console_init(void);

extern void console_set_current(console_t *c);
extern console_t *console_get_current(void);

extern void console_reset(console_t *c);
extern void console_putch(console_t *c,u08 ch);
extern void console_newline(console_t *c);

#endif