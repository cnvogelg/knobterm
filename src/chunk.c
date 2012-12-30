/*
* chunk.c - raw text chunk drawing
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

#include "chunk.h"
#include "screen.h"
#include "uart.h"

static u08 type;
static u08 pos_x;
static u08 pos_y;
static u08 width;
static u08 height;

static u08 cur_x;
static u08 cur_y;
static u16 bytes_left;
static u08 span;
static u08 cell;
static u08 cur_col;
static u08 cur_flags;

u08 chunk_handle_char(u08 ch)
{
  // something to process in raw chunk?
  if(bytes_left > 0) {
    // chunk with color?
    if((type=='B')&&(cell==0)) {
      cur_col = ch;
      cell = 1;
    } 
    // text char
    else {
      if(type == 0) {
        screen_putc(cur_x, cur_y, ch);
      } else {
        screen_putch(cur_x, cur_y, ch, cur_col, cur_flags);
      }
      span--;
      if(span == 0) {
        cur_x = pos_x;
        span = width;
        cur_y ++;
        // answer each span with a hash as a handshake
        uart_send('#');
      } else {
        cur_x ++;
      }
      cell = 0;
    }
    bytes_left--;
    return 1;
  } else {
    return 0;
  }
}

void chunk_define(u08 t, u08 x, u08 y, u08 w, u08 h)
{
  type = t;
  pos_x = x;
  pos_y = y;
  width = w;
  height = h;
}

void chunk_start(void)
{
  cur_x = pos_x;
  cur_y = pos_y;
  bytes_left = width * height;
  span = width;
  cell = 0;
  if(type == 'B') {
    bytes_left *= 2;
  }
  cur_flags = screen_get_flags();
  cur_col = screen_get_color();
}