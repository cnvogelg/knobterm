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
#include "timer.h"

#define EVENT_BUTTON_PRESS   'b'
#define EVENT_BUTTON_REPEAT  'r'
#define EVENT_JOG_UP         '+'
#define EVENT_JOG_DOWN       '-'
#define EVENT_OVERFLOW       '!'

#define MAX_EVENTS  16

typedef struct {
  u08 type;
  u08 value;
} event_t;

static event_t event_queue[MAX_EVENTS];
static u08 add_pos;
static u08 get_pos;
static u08 drops;
static u08 wait_event_flag;
static u16 wait_end_time;
static u08 wait_delay;

void input_init(void)
{
  jog_init();
  input_clear_queue();
}
   
void input_clear_queue(void)
{
  add_pos = 0;
  get_pos = 0;
  drops = 0;
}

static void write_event(const event_t *ev)
{  
  write_begin();
  write_cmd('i');
  write_ch(ev->type);
  write_hex_byte(ev->value);
  write_end();  
}

static void write_no_event(void)
{
  write_begin();
  write_cmd('i');
  write_ch('?');
  write_hex_byte(0);
  write_end();
}

static void write_drops(u08 drops)
{
  write_begin();
  write_cmd('i');
  write_ch('#');
  write_hex_byte(drops);
  write_end();
}

static event_t *get_next(void)
{
  if(add_pos == get_pos) {
    return 0;
  } 
  // get next event from queue
  else {
    event_t *ev = &event_queue[get_pos];
    get_pos ++;
    if(get_pos == MAX_EVENTS) {
      get_pos = 0;
    }
    return ev;
  }
}

void input_get_next_event(void)
{
  // drops are reported first
  if(drops) {
    write_drops(drops);
    drops = 0;
  }
  else {
    event_t *ev = get_next();
    // no more events
    if(ev == 0) {
      write_no_event();
    } 
    // get next event from queue
    else {
      write_event(ev);
    }
  }
}

void input_wait_for_next_event(u08 timeout_100ms)
{
  wait_event_flag = 1;
  wait_delay = timeout_100ms;
  wait_end_time = timer_10ms + timeout_100ms * 10;
}

static void add_event(u08 type, u08 value)
{
  // check for overflow
  u08 next_pos = add_pos + 1;
  if(next_pos == MAX_EVENTS) {
    next_pos = 0;
  }
  if(next_pos == get_pos) {
    drops ++;
  }
  // no overflow
  else {
    event_t * ev = &event_queue[add_pos];
    ev->type = type;
    ev->value = value;
    add_pos = next_pos;
  }
}

static void check_jog(void)
{
  // read jog wheel
  int8_t jog = jog_read();
  if(jog < 0) {
    add_event(EVENT_JOG_DOWN, -jog);
  }
  else if(jog > 0) {
    add_event(EVENT_JOG_UP, jog);
  }
}

static void check_button(void)
{
  u08 val = jog_button_press();
  if(val) {
    add_event(EVENT_BUTTON_PRESS, 0);
  }
  val = jog_button_repeat();
  if(val) {
    add_event(EVENT_BUTTON_REPEAT, 0);
  }
}

void input_handler(void)
{
  check_jog();
  check_button();
  
  // user is waiting for an event
  if(wait_event_flag) {
    // get next event
    event_t *ev = get_next();
    if(ev != 0) {
      write_event(ev);
      wait_event_flag = 0;
    }
    // check for time out
    else if(wait_delay > 0) {
      if(timer_10ms >= wait_end_time) {
        write_no_event();
        wait_event_flag = 0;
      }
    }
    
  }
}
