/*
 * input.c - handle input and generate events
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

#include "input.h"
#include "jog.h"
#include "write.h"

#define MODE_NONE 0
#define MODE_TRACK 1
#define MODE_SINGLE 2

static u08 mode;

void input_init(void)
{
  jog_init();
  mode = MODE_NONE;
}
   
void input_start(void)
{
  mode = MODE_TRACK;
}

void input_stop(void)
{
  mode = MODE_NONE;
}

void input_single(void)
{
  mode = MODE_SINGLE;
}

static void write_jog_event(int8_t val)
{
  u08 sign = '+';
  if(val < 0) {
    sign = '-';
    val = -val;
  }

  // limit movement
  if(val > 15) {
    val = 15;
  }
  
  write_begin();
  write_cmd('i');
  write_ch('j');
  write_ch(sign);
  write_hex_nybble(val);
  write_end();  
}

static void write_button_event(u08 num, u08 cmd)
{
  write_begin();
  write_cmd('i');
  write_ch('b');
  write_hex_nybble(num);
  write_ch(cmd);
  write_end();
}

static u08 check_jog(void)
{
  // read jog wheel
  int8_t jog = jog_read();
  if(jog != 0) {
    write_jog_event(jog);
    return 1;
  } else {
    return 0;
  }
}

static u08 check_button(void)
{
  u08 val = jog_button_press();
  if(val) {
    write_button_event(0,'p');
    return 1;
  }
  val = jog_button_repeat();
  if(val) {
    write_button_event(0,'r');
    return 1;
  }
  return 0;
}

void input_handler(void)
{
  if(mode != MODE_NONE) {
    // check jog
    if(check_jog()) {
      if(mode == MODE_SINGLE) {
        mode = MODE_NONE;
        return;
      }
    }
    // check button
    if(check_button()) {
      if(mode == MODE_SINGLE) {
        mode = MODE_NONE;
        return;
      }
    }
  }
}
