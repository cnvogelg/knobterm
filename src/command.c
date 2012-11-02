/*
 * command.c - parse command sequences
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
#include "command.h"
#include "write.h"
#include "console.h"
#include "util.h"
#include "screen.h"

#define CMD_OK          0
#define CMD_SYNTAX_ERR  1
#define CMD_UNKNOWN_ERR 2
#define CMD_NO_NYBBLE   3
#define CMD_NO_BYTE     4

void command_init(void)
{
   
}

static void error(u08 cmd, u08 num)
{
  write_begin();
  write_cmd('E');
  write_ch(cmd);
  write_hex_byte(num);
  write_end();
}

static void reply(u08 cmd, u08 num)
{
  write_begin();
  write_cmd(cmd);
  write_hex_byte(num);
  write_end();
}

static void version(void)
{
  write_begin();
  write_cmd('v');
  write_pstr(PSTR(VERSION));
  write_end();
}

static u08 color(const u08 *cmd, u08 len)
{
  if(len>3) {
    return CMD_SYNTAX_ERR;
  }
  
  console_t *c = console_get_current();

  // read color value
  if(len == 1) {
    reply('c', c->color);
  }
  else {
    u08 color;
    if(len == 2) {
      /* set foreground color */
      if(!parse_nybble(cmd[1], &color)) {
        return CMD_NO_NYBBLE;
      }
      color <<= 4;
    } else {
      /* set both colors */
      if(!parse_byte(cmd+1, &color)) {
        return CMD_NO_BYTE;
      }
    }
    c->color = color;
  }
  return CMD_OK;
}

static u08 flags(const u08 *cmd, u08 len)
{  
  console_t *c = console_get_current();

  if(len>3) {
    return CMD_SYNTAX_ERR;
  }

  // read color value
  if(len == 1) {
    reply('s', c->flags);
  }
  else {
    // try to parse value
    u08 flags = c->flags;
    if(len == 2) {
      switch(cmd[1]) {
        case 'x': 
          flags = (flags & ~FLAGS_FONT_2Y) | FLAGS_FONT_2X;
          break;
        case 'y': 
          flags = (flags & ~FLAGS_FONT_2X) | FLAGS_FONT_2Y;
          break;
        case 'b':
          flags |= FLAGS_FONT_2XY;
          break;
        case 'n':
          flags &= ~FLAGS_FONT_2XY;
          break;
        case 'A':
          flags = (flags & ~FLAGS_FONT_CHARSET_MASK) | 0 << (FLAGS_FONT_CHARSET_SHIFT);
          break;   
        case 'B':
          flags = (flags & ~FLAGS_FONT_CHARSET_MASK) | 1 << (FLAGS_FONT_CHARSET_SHIFT);
          break;   
      }
    } else {
      if(!parse_byte(cmd+1,&flags)) {
        return CMD_NO_BYTE;
      }
    }
    c->flags = flags;
  }
  return CMD_OK;
}

void command_parse(const u08 *cmd, u08 len)
{
  u08 result = 0;
  switch(cmd[0]) {
    case 'v':
      version();
      break;
    case 'c':
      result = color(cmd,len);
      break;
    case 'f':
      result = flags(cmd, len);
      break;
    default:
      result = CMD_UNKNOWN_ERR;
      break;
  }
  // report an error
  if(result != 0) {
    error(cmd[0],result);
  }
}
