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

/* the glorious C64 color table 
   see: http://www.pepto.de/projects/colorvic/
*/
static u16 palette[16] = {
  RGB(0x00,0x00,0x00), /* 0 black */
  RGB(0xFF,0xFF,0xFF), /* 1 white */
  RGB(0x68,0x37,0x2B), /* 2 red */
  RGB(0x70,0xA4,0xB2), /* 3 cyan */
  RGB(0x6F,0x3D,0x86), /* 4 purple */
  RGB(0x58,0x8D,0x43), /* 5 green */
  RGB(0x35,0x28,0x79), /* 6 blue */
  RGB(0xB8,0xC7,0x6F), /* 7 yellow */
  RGB(0x6F,0x4F,0x25), /* 8 orange */
  RGB(0x43,0x39,0x00), /* 9 brown */
  RGB(0x9A,0x67,0x59), /* a light red */
  RGB(0x44,0x44,0x44), /* b dark grey */
  RGB(0x6C,0x6C,0x6C), /* c grey */
  RGB(0x9A,0xD2,0x84), /* d light green */
  RGB(0x6C,0x5E,0xB5), /* e light blue */
  RGB(0x95,0x95,0x95)  /* f light grey */
};

static u08 cur_col = 0;
static u08 cur_flags = 0;
static u16 cur_fg = 0;
static u16 cur_bg = 0;
static u08 cur_font = 0;

const char *txt = "ABCDEFGHIJKLM";

#define NUM_FONTS 2
static const prog_uint8_t *font_table[] = {
  topaz_font,
  c64_font
};

void screen_init(void)
{
  display_set_font_data(font_table[0]);
  display_clear(0);
  display_set_font_scale(0,0);
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

void screen_update_color(u08 col)
{
  /* update color */
  if(col != cur_col) {
    cur_col = col;
    cur_fg = palette[(col>>4) & 0xf];
    cur_bg = palette[col & 0xf];
    display_set_color(cur_fg, cur_bg);
  }  
}

void screen_update_flags(u08 flags)
{
  /* update flags */
  if(flags != cur_flags) {
    cur_flags = flags;
    /* update scaling */
    u08 x2 = (cur_flags & FLAGS_FONT_2X);
    u08 y2 = (cur_flags & FLAGS_FONT_2Y);
    display_set_font_scale(x2, y2);
    /* update charset */
    u08 font = (flags & FLAGS_FONT_CHARSET_MASK) >> FLAGS_FONT_CHARSET_SHIFT;
    if((font != cur_font) && (font < NUM_FONTS)) {
      cur_font = font;
      display_set_font_data(font_table[cur_font]);
    }
  }  
}

void screen_putch(u08 x, u08 y, u08 ch, u08 col, u08 flags)
{
  screen_update_color(col);
  screen_update_flags(flags);
  
  u16 xp = x << 3;
  u16 yp = y << 3;
  display_draw_char(xp,yp,ch);
}

void screen_putc(u08 x, u08 y, u08 ch)
{  
  u16 xp = x << 3;
  u16 yp = y << 3;
  display_draw_char(xp,yp,ch);
}

void screen_get_size(u08 *w, u08 *h)
{
  display_get_size(w,h);
}
