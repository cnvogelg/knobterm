/*
 * cmd_query.c - query commands
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
#include "screen.h"

static void query_begin(u08 ch)
{
  write_begin();
  write_cmd('q');
  write_ch(ch);   
}

static void cmd_query_version(void)
{
  query_begin('v');
  write_pstr(PSTR(VERSION));
  write_end();
}

static void cmd_query_size(void)
{
  u08 w,h;
  
  screen_get_size(&w,&h);
 
  query_begin('s');  
  write_hex_byte(w);
  write_ch(',');
  write_hex_byte(h);
  write_end();
}

u08 cmd_query(const u08 *cmd, u08 len)
{
  u08 result = 0;
  switch(cmd[1]) {
   case 's': // size of terminal
     cmd_query_size();
     return CMD_OK;
   case 'v': // version
     cmd_query_version();
     return CMD_OK;
   default:
     return CMD_UNKNOWN_ERR;
  }
  /* always reply draw commands as they could take some time to complete */
  if(result == 0) {
    cmd_reply('d',0);
  }
  return result;
}
