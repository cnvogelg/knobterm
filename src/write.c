/*
 * write.c - write output from terminal to host
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
#include "write.h"
#include "uart.h"
#include "util.h"

#define MAX_LINE  80
   
static char line[MAX_LINE];
static u08 pos;

void write_begin(void)
{
  pos = 0;
}

void write_cmd(u08 c)
{
  if(pos < (MAX_LINE-1)) {
    line[pos++] = '@';
    line[pos++] = c;
  }
}

void write_ch(char c)
{  
  if(pos < MAX_LINE) {
    line[pos++] = c;
  }
}

void write_str(const char *str)
{
  while(*str && (pos < MAX_LINE)) {
    line[pos] = *str;
    str++;
    pos++;
  }
}

void write_pstr(PGM_P data)
{
  while(pos < MAX_LINE) {
    u08 d = pgm_read_byte_near(data);
    if(d==0) {
      break;
    }
    line[pos++] = d;
    data++;
  }
}

void write_hex_nybble(u08 v)
{
  if(pos < MAX_LINE) {
    line[pos] = nybble_to_hex(v);
    pos++;
  }
}

void write_hex_byte(u08 v)
{
  if(pos < (MAX_LINE-1)) {
    byte_to_hex(v, (u08 *)&line[pos]);
    pos+=2;
  }
}

void write_end(void)
{
  if(pos > 0) {
    /* write full line */
    for(u08 i=0;i<pos;i++) {
      uart_send(line[i]);
    }
    /* auto add line feed */
    if(line[pos-1] != '\n') {
      uart_send('\n');
    }
  }
}
