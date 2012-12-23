/*
 * command.h - parse command sequences
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

#ifndef COMMAND_H
#define COMMAND_H

#define CMD_OK          0
#define CMD_SYNTAX_ERR  1
#define CMD_UNKNOWN_ERR 2
#define CMD_NO_NYBBLE   3
#define CMD_NO_BYTE     4

extern void command_init(void);
extern void command_parse(const u08 *cmd, u08 len);
   
extern void cmd_error(u08 cmd, u08 num);
extern void cmd_reply(u08 cmd, u08 num);

#endif
