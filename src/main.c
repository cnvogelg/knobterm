/*
 * main.c - main loop
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
#include "uart.h"
#include "timer.h"
#include "display.h"
#include "fat/fatfs.h"
#include "util.h"
   
#include "read.h"
#include "write.h"
#include "console.h"
#include "screen.h"
#include "command.h"
#include "input.h"

static void read_image(void)
{
  FIL fsrc;
  FRESULT res;
  char filename[] = "main.raw";

  res = f_open(&fsrc, filename, FA_OPEN_EXISTING | FA_READ);
  if(res != FR_OK)
  {
      return;
  }

  for(u08 y=0;y<240;y++) {
    for(u16 x=0;x<320;x++) {
      u08 rgb[3];
      UINT read;
      res = f_read(&fsrc, &rgb, 3, &read);
      if(res != FR_OK)
        break;

      u16 color = RGB(rgb[0],rgb[1],rgb[2]);

      display_set_area(x,y,x,y);
      display_draw_start();
      display_draw_pixel(color);
      display_draw_stop();
    }
  }

  f_close(&fsrc);
}

void init(void)
{
  // board init. e.g. switch off watchdog
  board_init();  
  // setup timer
  timer_init();
  // setup serial
  uart_init();

  // input init
  input_init();
  // display init()
  display_init(2);
  
  // sdcard/fatfs init
  timer_set_10ms_func(fatfs_service);
  fatfs_init(2);
  
  // terminal init
  screen_init();
  read_init();
  console_init();
  command_init();
}

static void write_init_done(void)
{
  write_begin();
  write_cmd('i');
  write_end();
}

int main(void)
{  
  init();
  
  // mount sdcard
  if(fatfs_mount() == 0)
  {
    read_image();
    fatfs_umount();
  }

  //console_welcome();

  // write to console that init is done
  write_init_done();

  // main loop
  while(1) {
    read_serial();
    input_handler();
  }
  return 0;
}
