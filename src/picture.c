/*
 * picture.c - read picture from sd card
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

#include "picture.h"
#include "command.h"
#include "display.h"
#include "fat/fatfs.h"
#include "write.h"

#define BUF_SIZE 256

static u08 buf[BUF_SIZE];
static u16 buf_avail;
static u16 buf_pos;
static u16 width;
static u16 height;

static u08 parse_dec(const u08 *buf, u16 *out)
{
  u16 value = 0;
  u08 digits = 0;
  while(digits < 3) {
    u08 c = buf[digits];
    if((c<'0')||(c>'9')) {
      break;
    }
    c -= '0';
    value *= 10;
    value += c;
    digits++;
  }
  if(digits > 0) {
    *out = value;
  }
  return digits;
}

static u16 parse_header(const u08 *buf, u16 num)
{
  // "P6\n" header?
  if(num<3) {
    return PIC_HEADER_ERROR;
  }
  if((buf[0]!='P')||(buf[1]!='6')||(buf[2]!='\n')) {
    return PIC_HEADER_ERROR;
  }
  u16 hdr_pos = 3;
  
  // parse header lines
  u08 num_lines = 2;
  while(num_lines > 0) {
    // find newline
    u16 nl_pos = hdr_pos;
    while(nl_pos < num) {
      if(buf[nl_pos]=='\n') {
        break;
      }
      nl_pos ++;
    }
    // no newline found!
    if(nl_pos >= num) {
      return PIC_HEADER_TOO_LONG;
    }
    
    // ignore comment line
    if(buf[hdr_pos]!='#') {
      if(num_lines == 2) {
        // parse width
        u08 digits = parse_dec(buf+hdr_pos, &width);
        if(digits == 0) {
          return PIC_WIDTH_ERROR;
        }
        // parse height
        digits = parse_dec(buf+hdr_pos+digits+1, &height);
        if(digits == 0) {
          return PIC_HEIGHT_ERROR;
        }
      }
      num_lines --;
    }
    // skip line
    hdr_pos = nl_pos + 1;
  }

  // set buffer to start
  buf_pos = hdr_pos;
  buf_avail = num - hdr_pos;
  return CMD_OK;
}

static u08 read_byte(FIL *fh,u08 *b)
{
  if(buf_avail == 0) {
    UINT read;
    FRESULT res = f_read(fh, &buf, BUF_SIZE, &read);
    if(res != FR_OK) {
      return PIC_READ_ERROR;
    }
    buf_avail = read;
    buf_pos = 0;
  }
  
  *b = buf[buf_pos];
  buf_pos ++;
  buf_avail --;
  return CMD_OK;
}

static u08 read_pixel(FIL *fh,u08 *rgb)
{
  for(int i=0;i<3;i++) {
    u08 res = read_byte(fh,rgb+i);
    if(res != CMD_OK) {
      return res;
    }
  }
  return CMD_OK;
}

static u08 load_data(FIL *fh,u16 xp, u16 yp)
{
  u08 result = CMD_OK;
  
  for(u16 y=0;y<height;y++) {
    for(u16 x=0;x<width;x++) {
      u08 rgb[3];
      if(result == CMD_OK) {
        result = read_pixel(fh,rgb);
      }
      u16 color = RGB(rgb[0],rgb[1],rgb[2]);

      display_set_area(xp+x,yp+y,xp+x,yp+y);
      display_draw_start();
      display_draw_pixel(color);
      display_draw_stop();
    }
  }
  
  return result;
}

u08 picture_load(const char *filename, u08 x, u08 y)
{
  FIL fsrc;
  FRESULT res;
  u08 result = CMD_OK;
  
  // try to mount sd card
  if(fatfs_mount() !=0) {
    result = PIC_MOUNT_ERROR;
  }
  else {
    // open file
    res = f_open(&fsrc, filename, FA_OPEN_EXISTING | FA_READ);
    if(res == FR_OK)
    {
      // read header
      UINT read;
      res = f_read(&fsrc, &buf, BUF_SIZE, &read);
      if(res == FR_OK) {
        // parse header
        result = parse_header(buf, read);
        if(result == CMD_OK) {
          // prepare display
          u16 xp = x * 8;
          u16 yp = y * 8;          
          // load bitmap data
          result = load_data(&fsrc,xp,yp);
        }
      } else {
        result = PIC_READ_ERROR;
      }
      // close file
      f_close(&fsrc);
    }
  
    // unmount
    fatfs_umount();
  }
  return result;
}
