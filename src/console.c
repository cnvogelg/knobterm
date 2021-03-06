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
#include "draw.h"

static console_t c = {
  .width = 40,
  .height = 30,
  .top_x = 0,
  .top_y = 0,
  .cursor_x = 0,
  .cursor_y = 0,
  .color = 0x10,
  .flags = 0
};
static console_t *cur_console = &c;

void console_init(void)
{
  cur_console = &c;
  console_clear(cur_console, cur_console->color);
}

void console_welcome(void)
{
  console_putr(cur_console,PSTR("knobterm " VERSION "\nby lallafa\n\nREADY.\n"));
}

void console_set_current(console_t *c)
{
  cur_console = c;
}

console_t *console_get_current(void)
{
  return cur_console;
}

void console_clear(console_t *c, u08 col)
{
  c->cursor_x = 0;
  c->cursor_y = 0;
  screen_erase(c->top_x, c->top_y, c->width, c->height, col);
}

void console_newline(console_t *c)
{
  c->cursor_x = 0;
  /* end of last line? */
  if(c->cursor_y < (c->height-1)) {
    c->cursor_y ++;
    if(c->flags & FLAGS_FONT_2Y) {
      c->cursor_y ++;
    }
  }
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
    
    /* advance cursor */
    console_next_char(c);
  }
  /* newline? */
  else if(ch == '\n') {
    console_newline(c);
  }
}

void console_putr(console_t *c, PGM_P pstr)
{
  while(1) {
     u08 ch = pgm_read_byte_near(pstr);
     if(ch == 0) {
       break;
     }
     console_putch(c, ch);
     pstr++;
   }
}

void console_border(console_t *c, u08 t, u08 x, u08 y, u08 w, u08 h)
{
  screen_update_color(c->color);
  draw_border(t, x, y, w, h);
}

void console_rect(console_t *c, u08 ch, u08 x, u08 y, u08 w, u08 h)
{
  screen_update_flags(c->flags);
  screen_update_color(c->color);
  draw_rect(ch, x, y, w, h);
}

void console_grid(console_t *c, u08 t, u08 x, u08 y, u08 nx, u08 ny, u08 dx, u08 dy)
{
  screen_update_color(c->color);
  draw_grid(t, x, y, nx, ny, dx, dy);
}

void console_h_line(console_t *c, u08 ch, u08 x, u08 y, u08 len)
{
  screen_update_flags(c->flags);
  screen_update_color(c->color);
  draw_h_line(ch, x, y, len);
}  

void console_v_line(console_t *c, u08 ch, u08 x, u08 y, u08 len)
{
  screen_update_flags(c->flags);
  screen_update_color(c->color);
  draw_v_line(ch, x, y, len);
}  

void console_goto(console_t *c, u08 x, u08 y)
{
  if(x >= c->width) {
    x = c->width - 1;
  }
  if(y >= c->height) {
    y = c->height - 1;
  }
  c->cursor_x = x;
  c->cursor_y = y;
}

