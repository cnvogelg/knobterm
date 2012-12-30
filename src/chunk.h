/*
* chunk.h - raw text chunk drawing
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

#ifndef CHUNK_H
#define CHUNK_H

#include "global.h"
   
void chunk_define(u08 ch, u08 x, u08 y, u08 w, u08 h);
void chunk_start(void);
u08  chunk_handle_char(u08 ch);

#endif
