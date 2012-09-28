/*
 * console.c - console output
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

#include <string.h>

#include "global.h"
#include "console.h"
#include "screen.h"


static u08 text_buf[40*20];
static console_t c = {
  .width = 40,
  .height = 20,
  .top_x = 0,
  .top_y = 0,
  .cursor_x = 0,
  .cursor_y = 0,
  .buffer = text_buf,
  .color = 0x10,
  .flags = 0,
  .cursor_color = 0x03
};
static console_t *cur_console = &c;

void console_init(void)
{
  cur_console = &c;
  console_reset(cur_console);
}

void console_set_current(console_t *c)
{
  cur_console = c;
}

console_t *console_get_current(void)
{
  return cur_console;
}

void console_reset(console_t *c)
{
  if(c->buffer != 0) {
    //memset(c->buffer, ' ', c->width * c->height);
  }
  c->cursor_x = 0;
  c->cursor_y = 0;
  screen_erase(c->top_x, c->top_y, c->width, c->height, c->color);
  
  /* draw initial cursor */
  if(c->cursor_color != 0) {
    screen_putch(c->top_x, c->top_y, ' ',c->cursor_color,c->flags);
  }
}

void console_scroll_up(console_t *c)
{
  /* only possible if using a buffer */
  if(c->buffer == 0) {
    return;
  }
  
  u08 *tgt = c->buffer;
  u08 *src = c->buffer + c->width;
  u08 ty = c->top_y;
  for(u08 y=1;y<c->height;y++) {
    u08 tx = c->top_x;
    /* copy line above */
    for(u08 x=0;x<c->width;x++) {
      *tgt = *src;
      screen_putch(tx,ty,*src,c->color,c->flags);
      src++;
      tgt++;
      tx++;
    }
    ty++;
  }
  
  /* clear bottom line */
  u08 tx = c->top_x;
  for(u08 x=0;x<c->width;x++) {
    *(tgt++) = ' ';
    screen_putch(tx,ty,' ',c->color,c->flags);
    tx++;
  }
}

void console_draw_cursor(console_t *c, u08 show)
{
  /* draw updated cursor */
  if(c->cursor_color != 0) {
    u08 x = c->top_x + c->cursor_x;
    u08 y = c->top_y + c->cursor_y;
    
    u08 ch = ' ';
    if(c->buffer != 0) {
      u16 off = c->cursor_x + (c->cursor_y * c->width);
      ch = c->buffer[off];
    }
    
    u08 color;
    if(show) {
      color = c->cursor_color;
    } else {
      color = c->color;
    }
    screen_putch(x,y,ch,color,c->flags);
  }  
}

void console_newline(console_t *c)
{
  /* clear old cursor */
  if(c->cursor_x < c->width) {
    console_draw_cursor(c,0);
  }
  
  c->cursor_x = 0;
  /* end of last line? */
  if(c->cursor_y == (c->height-1)) {
    console_scroll_up(c);
  } 
  /* move to next line */
  else {
    c->cursor_y ++;
    if(c->flags & FLAGS_FONT_2Y) {
      c->cursor_y ++;
    }
  }
  console_draw_cursor(c,1);
}

void console_next_char(console_t *c)
{
  /* move */
  c->cursor_x ++;
  if(c->flags & FLAGS_FONT_2X) {
    c->cursor_x ++;
  }
  if(c->cursor_x >= c->width) {
    console_newline(c);
  } else {
    console_draw_cursor(c,1);
  }
}

void console_putch(console_t *c, u08 ch)
{
  /* normal char */
  if(ch > 31) {
    /* put on screen */
    u08 x = c->top_x + c->cursor_x;
    u08 y = c->top_y + c->cursor_y;
    screen_putch(x,y,ch,c->color,c->flags);
    
    /* store in buffer */
    if(c->buffer != 0) {
      u16 off = c->cursor_x + (c->cursor_y * c->width);
      c->buffer[off] = ch;
    }
    
    /* advance cursor */
    console_next_char(c);
  }
}
