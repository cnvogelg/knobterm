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
  /* corners: 0=TL, 1=TR, 2=BL, 3=BR,  bars: 4=HT, 5=HB, 6=VL, 7=VR */
  { 0xb0, 0xae, 0xad, 0xbd, 0x80, 0x80, 0x82, 0x82 },
  { 0x95, 0x89, 0x8a, 0x8b, 0x80, 0x80, 0x82, 0x82 },
  { 0x8f, 0x90, 0x8c, 0xba, 0xb7, 0xaf, 0xb4, 0xaa }
};

#define NUM_GRIDS 1
static const u08 grid_chars[][7] = {
  /* tee: 0=HD, 1=HU, 2=VR, 3=VL, 4=X 5=HC 6=VC*/
  { 0xb2, 0xb1, 0xab, 0xb3, 0x9b, 0x80, 0x82 }
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
    screen_putc(x+i,y,bc[4]); // top row
    screen_putc(x+i,y+h+1,bc[5]); // bot row
  }
  for(u08 i=1;i<=h;i++) {
    screen_putc(x,y+i,bc[6]); // left col
    screen_putc(x+w+1,y+i,bc[7]); // right col
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

/*
cl - left border char
cs - space border char (>7 = none)
ci - internal grid char
cr - right border char
*/
static void draw_grid_span(u08 x, u08 y, u08 nx, u08 dx, u08 cl, u08 cs, u08 cr, u08 ci)
{
  u08 px;

  screen_putc(x,y,cl); 
  px = x + 1;
  for(u08 i=0;i<nx;i++) {
    if(i > 0) {
      screen_putc(px++,y,ci);
    } 
    if(cs != 0xff) {
      for(u08 j=0;j<dx;j++) {
        screen_putc(px++,y,cs);
      }
    } else {
      px += dx;
    }
  }
  screen_putc(px,y,cr);
}

void draw_grid(u08 type, u08 x, u08 y, u08 nx, u08 ny, u08 dx, u08 dy)
{
  if(type >= NUM_GRIDS) {
    type = 0;
  }
  const u08 *bc = border_chars[type];
  const u08 *gc = grid_chars[type];
  
  // force font #1, no scale
  u08 flags = (1 << FLAGS_FONT_CHARSET_SHIFT);
  screen_update_flags(flags);
  
  /* top row */
  draw_grid_span(x, y, nx, dx, bc[0], bc[4], bc[1], gc[0]);
  y++;
    
  /* mid row */
  for(u08 j=0;j<ny;j++) {
    /* row with grid */
    if(j > 0) {
      draw_grid_span(x, y, nx, dx, gc[2], gc[5], gc[3], gc[4]);
      y++;
    }
    /* row without grid */
    for(u08 k=0;k<dy;k++) {
      draw_grid_span(x, y, nx, dx, bc[6], ' ', bc[7], gc[6]);
      y++;
    }
  }
  
  /* bot row */
  draw_grid_span(x, y, nx, dx, bc[2], bc[5], bc[3], gc[1]);
}

void draw_h_line(u08 ch, u08 x, u08 y, u08 w)
{
  for(u08 i=0;i<w;i++) {
    screen_putc(x, y, ch);
    x++;
  }
}

void draw_v_line(u08 ch, u08 x, u08 y, u08 h)
{
  for(u08 i=0;i<h;i++) {
    screen_putc(x, y, ch);
    y++;
  }
}

