/*
 * draw.c - Graphics drawing with font
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

#include "draw.h"
#include "screen.h"

#define NUM_BORDERS 3
static const u08 border_chars[][8] ={
  /* corners: TL, TR, BL, BR,  bars: HT, HB, VL, VR */
  { 0xb0, 0xae, 0xad, 0xbd, 0x80, 0x80, 0x82, 0x82 },
  { 0x95, 0x89, 0x8a, 0x8b, 0x80, 0x80, 0x82, 0x82 },
  { 0x8f, 0x90, 0x8c, 0xba, 0xb7, 0xaf, 0xb4, 0xaa }
};

void draw_border(u08 type, u08 x, u08 y, u08 w, u08 h)
{
  if(type >= NUM_BORDERS) {
    type = 0;
  }
  
  const u08 *bc = border_chars[type];
  
  // force font #1, no scale
  u08 flags = (1 << FLAGS_FONT_CHARSET_SHIFT);
  screen_update_flags(flags);
  
  screen_putc(x,y,bc[0]);
  screen_putc(x+w+1,y,bc[1]);
  screen_putc(x,y+h+1,bc[2]);
  screen_putc(x+w+1,y+h+1,bc[3]);
  
  for(u08 i=1;i<=w;i++) {
    screen_putc(x+i,y,bc[4]);
    screen_putc(x+i,y+h+1,bc[5]);
  }
  for(u08 i=1;i<=h;i++) {
    screen_putc(x,y+i,bc[6]);
    screen_putc(x+w+1,y+i,bc[7]);
  }
}

void draw_rect(u08 ch, u08 x, u08 y, u08 w, u08 h)
{ 
  for(u08 j=0;j<h;j++) {
    for(u08 i=0;i<w;i++) {
      screen_putc(x+i,y+j,ch);
    }
  }
}

