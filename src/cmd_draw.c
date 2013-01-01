/*
* cmd_draw.c - draw commands
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

#include "cmd_query.h"
#include "command.h"
#include "write.h"
#include "util.h"
#include "console.h"
#include "chunk.h"

static u08 vals[6];

static u08 parse_vals(const u08 *arg, u08 num)
{
  for(u08 i=0;i<num;i++) {
   if(!parse_byte(arg,&vals[i])) {
     return CMD_NO_BYTE;
   }
   arg+=2;
  }
  return CMD_OK;
}

static u08 cmd_draw_border(const u08 *cmd, u08 len)
{
  if(len != 10) {
   return CMD_SYNTAX_ERR;
  }
  
  // parse type
  u08 t;
  switch(cmd[1]) {
   case 'A': t=0; break;
   case 'B': t=1; break;
   case 'C': t=2; break;
   default: return CMD_SYNTAX_ERR;
  }
  
  // parse coords
  u08 res = parse_vals(cmd+2,4);
  if(res != CMD_OK) {
   return res;
  }
  
  console_border(console_get_current(), t, vals[0], vals[1], vals[2], vals[3]);
  return CMD_OK;
}

static u08 cmd_draw_rect(const u08 *cmd, u08 len)
{
  if(len!=10) {
   return CMD_SYNTAX_ERR;
  }

  // char to draw
  u08 t = cmd[1];
  
  // parse coords
  u08 res = parse_vals(cmd+2,4);
  if(res != CMD_OK) {
   return res;
  }
  
  console_rect(console_get_current(), t, vals[0], vals[1], vals[2], vals[3]);
  return CMD_OK;
}

static u08 cmd_chunk_define(const u08 *cmd, u08 len)
{
  if(len!=10) {
   return CMD_SYNTAX_ERR;
  }

  // char to draw
  u08 t = cmd[1];
  
  // parse coords
  u08 res = parse_vals(cmd+2,4);
  if(res != CMD_OK) {
   return res;
  }
  
  chunk_define(t, vals[0], vals[1], vals[2], vals[3]);
  return CMD_OK;
}

static u08 cmd_draw_grid(const u08 *cmd, u08 len)
{
  if(len!=14) {
   return CMD_SYNTAX_ERR;
  }
  
  // parse type
  u08 t;
  switch(cmd[1]) {
   case 'A': t=0; break;
   default: return CMD_SYNTAX_ERR;
  }
  
  // parse coords
  u08 res = parse_vals(cmd+2,6);
  if(res != CMD_OK) {
   return res;
  }
  
  console_grid(console_get_current(), t, vals[0], vals[1], vals[2], vals[3], vals[4], vals[5]);
  return CMD_OK;
}

static u08 cmd_draw_line(const u08 *cmd, u08 len)
{
  // syntax (h|v)<char><b:x><b:y><b:len>
  if(len < 8) {
   return CMD_SYNTAX_ERR;
  }
  
  u08 mode = cmd[0];
  u08 ch = cmd[1];

  // parse coords
  u08 res = parse_vals(cmd+2,3);
  if(res != CMD_OK) {
   return res;
  }
  
  console_t *c = console_get_current();
  if(mode == 'h') {
   console_h_line(c, ch, vals[0], vals[1], vals[2]);
  }
  else {
   console_v_line(c, ch, vals[0], vals[1], vals[2]);    
  }
  return CMD_OK;
}

u08 cmd_draw(const u08 *cmd, u08 len)
{
  u08 result = 0;
  switch(cmd[1]) {
   case 'b':
     result = cmd_draw_border(cmd+1,len-1);
     break;
   case 'r':
     result = cmd_draw_rect(cmd+1,len-1);
     break;
   case 'g':
     result = cmd_draw_grid(cmd+1,len-1);
     break;
   case 'h':
   case 'v':
     result = cmd_draw_line(cmd+1, len-1);
     break;
   case 'c':
     result = cmd_chunk_define(cmd+1, len-1);
     break;
   case 'C':
     /* no reply on start */
     chunk_start();
     return CMD_NO_REPLY;
   default:
     return CMD_UNKNOWN_ERR;
  }
  return result;
}

