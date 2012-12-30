/*
 * read.c - read input for terminal
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
#include "read.h"
#include "uart.h"
#include "console.h"
#include "command.h"
#include "chunk.h"

#define MAX_LINE 80

static u08 line[MAX_LINE];
static u08 line_pos = 0;
static u08 begin_cmd = '@';
static u08 end_cmd = ';';
static u08 in_cmd = 0;

void read_init(void)
{
  line_pos = 0;
}

void read_putch(u08 ch)
{
  /* in a command sequence? */
  if(in_cmd) {
    /* command just ended */
    if((ch == end_cmd)||(ch == '\n')) {
      line[line_pos] = 0;
      in_cmd = 0;
      if(line_pos > 0) {
        command_parse(line, line_pos);
        /* start new command */
        line_pos = 0;
      }
    }
    /* begin of a new command */
    else if(ch == begin_cmd) {
      /* single char is quoted */
      if(line_pos == 0) {
        console_putch(console_get_current(),'@');
        in_cmd = 0;
      } 
      /* other sequence is a command */
      else {
        line[line_pos] = 0;
        command_parse(line, line_pos);
        /* start next command */
        line_pos = 0;
      }
    }
    /* ignore CR */
    else if(ch == '\r') {
      // nothing
    }
    /* in command */
    else if(line_pos < (MAX_LINE-1)) {
      line[line_pos++] = ch;
    }
  }
  /* put on console */
  else {
    /* does a command start? */
    if(ch == begin_cmd) {
      in_cmd = 1;
    }
    /* not command character? */
    else {
      console_putch(console_get_current(),ch);
    }
  }
}

void read_serial(void)
{
  if(uart_read_data_available()) {
    u08 ch = uart_read();
    if(!chunk_handle_char(ch)) {
      read_putch(ch);
    }
  }
}
