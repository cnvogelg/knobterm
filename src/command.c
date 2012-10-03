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

void command_init(void)
{
   
}

static void error(u08 num)
{
  write_begin();
  write_cmd('E');
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

void command_parse(const u08 *cmd)
{
  switch(cmd[0]) {
    case 'v':
      version();
      break;
    default:
      error(0);
      break;
  }
}
