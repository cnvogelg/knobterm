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
#include "input.h"
   
#include "cmd_query.h"
#include "cmd_draw.h"

void command_init(void)
{
   
}

void cmd_error(u08 cmd, u08 num)
{
  write_begin();
  write_cmd('E');
  write_ch(cmd);
  write_hex_byte(num);
  write_end();
}

void cmd_reply(u08 cmd, u08 num)
{
  write_begin();
  write_cmd(cmd);
  write_hex_byte(num);
  write_end();
}

static void cmd_sync(void)
{
  write_begin();
  write_cmd('s');
  write_end();
}

static u08 cmd_color(const u08 *cmd, u08 len)
{
  if(len>3) {
    return CMD_SYNTAX_ERR;
  }
  
  console_t *c = console_get_current();

  // read color value
  if(len == 1) {
    cmd_reply('c', c->color);
  }
  else {
    u08 color;
    if(len == 2) {
      /* set foreground color */
      if(!parse_nybble(cmd[1], &color)) {
        return CMD_NO_NYBBLE;
      }
      c->color = (color << 4) | (c->color & 0xf);
    } else {
      /* set both colors */
      if(cmd[1] != '-') {
        if(!parse_byte(cmd+1, &color)) {
          return CMD_NO_BYTE;
        }
        c->color = color;
      } else {
        /* set background color */
        if(!parse_nybble(cmd[2], &color)) {
          return CMD_NO_NYBBLE;
        }
        c->color = (c->color & 0xf0) | color;
      }
    }
  }
  return CMD_OK;
}

static u08 cmd_flags(const u08 *cmd, u08 len)
{  
  console_t *c = console_get_current();

  if(len>3) {
    return CMD_SYNTAX_ERR;
  }

  // read color value
  if(len == 1) {
    cmd_reply('s', c->flags);
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

static u08 cmd_goto(const u08 *cmd, u08 len)
{
  if(len > 5) {
    return CMD_SYNTAX_ERR;
  }
  
  u08 x,y;
  if(!parse_byte(cmd+1,&x)) {
    return CMD_NO_BYTE;
  }
  if(!parse_byte(cmd+3,&y)) {
    return CMD_NO_BYTE;
  }
  
  console_t *c = console_get_current();
  console_goto(c, x, y);
  return CMD_OK;
}

static u08 cmd_erase(const u08 *cmd, u08 len)
{
  if(len > 2) {
    return CMD_SYNTAX_ERR;
  }
  
  u08 col = 0;
  if(len == 2) {
    if(!parse_nybble(cmd[1],&col)) {
      return CMD_NO_NYBBLE;
    }
  }
  
  console_t *c = console_get_current();
  console_clear(c, col);
  
  cmd_reply(cmd[0],0);
  return CMD_OK;
}

static u08 cmd_input(const u08 *cmd, u08 len)
{
  switch(cmd[1]) {
    case 'g': // get next event or no event
      input_get_next_event();
      break;
    case 'w': // wait for next event or timeout (n!=0)
      if(len==4) {
        u08 timeout_100ms;
        if(!parse_byte(cmd+2,&timeout_100ms)) {
          return CMD_NO_BYTE;
        }
        input_wait_for_next_event(timeout_100ms);
      } else {
        return CMD_SYNTAX_ERR;
      }
    case 'c':
      input_clear_queue();
      break;
    default:
      return CMD_UNKNOWN_ERR;
  }
  return CMD_OK;
}

void command_parse(const u08 *cmd, u08 len)
{
  u08 result = 0;
  switch(cmd[0]) {
    case 'c':
      result = cmd_color(cmd,len);
      break;
    case 'f':
      result = cmd_flags(cmd, len);
      break;
    case 'd':
      result = cmd_draw(cmd, len);
      break;
    case 'g':
      result = cmd_goto(cmd, len);
      break;
    case 'e':
      result = cmd_erase(cmd, len);
      break;
    case 's':
      cmd_sync();
      break;
    case 'q':
      result = cmd_query(cmd, len);
      break;
    case 'i':
      result = cmd_input(cmd, len);
      break;
    default:
      result = CMD_UNKNOWN_ERR;
      break;
  }
  // report an error
  if(result != 0) {
    cmd_error(cmd[0],result);
  }
}
