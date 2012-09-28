/*
 * screen.c - text and image drawing
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

#include "global.h"
#include "screen.h"
#include "fonts.h"
#include "display.h"

static u16 palette[16] = {
  COLOR_BLACK,
  COLOR_WHITE,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
  COLOR_LGRAY,
  COLOR_GRAY,
  COLOR_DGRAY    
};

static u08 cur_col = 0;
static u08 cur_flags = 0;
static u16 cur_fg = 0;
static u16 cur_bg = 0;

const char *txt = "ABCDEFGHIJKLM";

void screen_init(void)
{
  display_set_font_data(topaz_font);
  display_clear(0);
  display_set_font_scale(0,0);
  
  screen_puts(0,28,"knobterm",FGBG(1,0),FLAGS_FONT_2XY);
  screen_puts(20,28,VERSION,FGBG(1,0),0);
  screen_puts(20,29,"by lallafa",FGBG(1,0),0);
}

void screen_clear(u08 col)
{
  u16 bg = palette[col & 0xf];
  display_clear(bg);
}
  
void screen_erase(u08 x, u08 y, u08 w, u08 h, u08 col)
{
  u16 bg = palette[col & 0xf];
  u16 xp = x << 3;
  u16 yp = y << 3;
  u16 wp = w << 3;
  u16 hp = h << 3;
  display_draw_rect(xp,yp,wp,hp,bg);
}

void screen_putch(u08 x, u08 y, u08 ch, u08 col, u08 flags)
{
  /* update color */
  if(col != cur_col) {
    cur_col = col;
    cur_fg = palette[(col>>4) & 0xf];
    cur_bg = palette[col & 0xf];
    display_set_color(cur_fg, cur_bg);
  }
  /* update flags */
  if(flags != cur_flags) {
    cur_flags = flags;
    u08 x2 = (cur_flags & FLAGS_FONT_2X);
    u08 y2 = (cur_flags & FLAGS_FONT_2Y);
    display_set_font_scale(x2, y2);
  }
  
  u16 xp = x << 3;
  u16 yp = y << 3;
  display_draw_char(xp,yp,ch);
}

void screen_puts(u08 x, u08 y, const char *str, u08 col, u08 flags)
{
  while(*str) {
    u08 ch = (u08)*str;
    screen_putch(x,y,ch,col,flags);
    if(flags & FLAGS_FONT_2X) x++;
    x++;
    str++;
  }
}
