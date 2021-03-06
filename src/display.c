/*
 * display.c - Control display
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

#include "display.h"
#include <util/delay.h>
   
#define LED_PIN        1
#define LED_ENABLE()   PORTB |=  (1<<1)
#define LED_DISABLE()  PORTB &= ~(1<<1)

#define RST_PIN        0
#define RST_DISABLE()  PORTB |=  (1<<0)
#define RST_ENABLE()   PORTB &= ~(1<<0)

#define CS_PIN         7
#define CS_DISABLE()   PORTD |=  (1<<7)
#define CS_ENABLE()    PORTD &= ~(1<<7)

#define MOSI_PIN       3
#define MOSI_HIGH()    PORTB |=  (1<<3)
#define MOSI_LOW()     PORTB &= ~(1<<3)

#define MISO_PIN       4
#define MISO_READ()    (PINB & (1<<4))

#define CLK_PIN        5
#define CLK_HIGH()     PORTB |=  (1<<5)
#define CLK_LOW()      PORTB &= ~(1<<5)

#define LCD_ID          (0)
#define LCD_DATA        ((0x72)|(LCD_ID<<2))
#define LCD_REGISTER    ((0x70)|(LCD_ID<<2))

#define SS_PIN         2

void display_init(u08 clock_div)
{
    // LED/RESET are output, CLK+MOSI
    DDRB  |= _BV(LED_PIN) | _BV(RST_PIN) | _BV(CLK_PIN) | _BV(MOSI_PIN);
    // MISO is input
    DDRB  &= ~_BV(MISO_PIN);
    // LED=HI, MISO=Pullop
    PORTB |= _BV(LED_PIN) | _BV(MISO_PIN);
    PORTB &= ~(_BV(RST_PIN) | _BV(CLK_PIN));
    
    // CS is output and HI
    DDRD  |= _BV(CS_PIN);
    PORTD |= _BV(CS_PIN);

    // make sure SS_PIN is pull up
    if(!(DDRB & (1<<SS_PIN))) //SS is input
    {
        PORTB |= (1<<SS_PIN); //pull-up on
    }

    //init hardware spi
    switch(clock_div)
    {
    case 2:
      SPCR = (1<<SPE)|(1<<MSTR); //enable SPI, Master, clk=Fcpu/4
      SPSR = (1<<SPI2X); //clk*2 = Fcpu/2
      break;
    case 4:
      SPCR = (1<<SPE)|(1<<MSTR); //enable SPI, Master, clk=Fcpu/4
      SPSR = (0<<SPI2X); //clk*2 = off
      break;
    case 8:
      SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //enable SPI, Master, clk=Fcpu/16
      SPSR = (1<<SPI2X); //clk*2 = Fcpu/8
      break;
    case 16:
      SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //enable SPI, Master, clk=Fcpu/16
      SPSR = (0<<SPI2X); //clk*2 = off
      break;
    case 32:
      SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1); //enable SPI, Master, clk=Fcpu/64
      SPSR = (1<<SPI2X); //clk*2 = Fcpu/32
      break;
    }
    
    display_reset();
}

static void wr_spi(u08 data)
{
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));
}

static void wr_cmd(u08 reg, u08 param)
{
    CS_ENABLE();
    wr_spi(LCD_REGISTER);
    wr_spi(reg);
    CS_DISABLE();

    CS_ENABLE();
    wr_spi(LCD_DATA);
    wr_spi(param);
    CS_DISABLE();
}

#if 0
static void wr_data(u16 data)
{
    CS_ENABLE();
    wr_spi(LCD_DATA);
    wr_spi(data>>8);
    wr_spi(data);
    CS_DISABLE();
}
#endif

void display_reset(void)
{
    //reset
    CS_DISABLE();
    RST_ENABLE();
    _delay_ms(50);
    RST_DISABLE();
    _delay_ms(50);

    //driving ability
    wr_cmd(0xEA, 0x0000);
    wr_cmd(0xEB, 0x0020);
    wr_cmd(0xEC, 0x000C);
    wr_cmd(0xED, 0x00C4);
    wr_cmd(0xE8, 0x0040);
    wr_cmd(0xE9, 0x0038);
    wr_cmd(0xF1, 0x0001);
    wr_cmd(0xF2, 0x0010);
    wr_cmd(0x27, 0x00A3);

    //power voltage
    wr_cmd(0x1B, 0x001B);
    wr_cmd(0x1A, 0x0001);
    wr_cmd(0x24, 0x002F);
    wr_cmd(0x25, 0x0057);

    //VCOM offset
    wr_cmd(0x23, 0x008D); //for flicker adjust

    //power on
    wr_cmd(0x18, 0x0036);
    wr_cmd(0x19, 0x0001); //start osc
    wr_cmd(0x01, 0x0000); //wakeup
    wr_cmd(0x1F, 0x0088);
    _delay_ms(5);
    wr_cmd(0x1F, 0x0080);
    _delay_ms(5);
    wr_cmd(0x1F, 0x0090);
    _delay_ms(5);
    wr_cmd(0x1F, 0x00D0);
    _delay_ms(5);

    //color selection
    wr_cmd(0x17, 0x0005); //0x0005=65k, 0x0006=262k

    //panel characteristic
    wr_cmd(0x36, 0x0000);

    //display on
    wr_cmd(0x28, 0x0038);
    _delay_ms(40);
    wr_cmd(0x28, 0x003C);

    //display options
    display_set_orientation(0);
}

void display_enable(u08 on)
{
  if(on) {
    wr_cmd(0x28, 0x0038);
    _delay_ms(40);
    wr_cmd(0x28, 0x003C);  
  } else {
    wr_cmd(0x28, 0x0038);
    _delay_ms(40);
    wr_cmd(0x28, 0x0004);
  }
}

void display_backlight(u08 on)
{
  if(on) {
    LED_ENABLE();
  } else {
    LED_DISABLE();
  }
}

u16 display_width;
u16 display_height;
u08 display_orientation; 

void display_set_orientation(u08 o)
{
    switch(o)
    {
        case DISPLAY_ORIENTATION_0:
        display_orientation = DISPLAY_ORIENTATION_0;
        display_width  = 320;
        display_height = 240;
        wr_cmd(0x16, 0x00A8); //MY=1 MX=0 MV=1 ML=0 BGR=1
        break;

        case DISPLAY_ORIENTATION_90:
        display_orientation = DISPLAY_ORIENTATION_90;
        display_width  = 240;
        display_height = 320;
        wr_cmd(0x16, 0x0008); //MY=0 MX=0 MV=0 ML=0 BGR=1
        break;

        case DISPLAY_ORIENTATION_180:
        display_orientation = DISPLAY_ORIENTATION_180;
        display_width  = 320;
        display_height = 240;
        wr_cmd(0x16, 0x0068); //MY=0 MX=1 MV=1 ML=0 BGR=1
        break;

        case DISPLAY_ORIENTATION_270:
        display_orientation = DISPLAY_ORIENTATION_270;
        display_width  = 240;
        display_height = 320;
        wr_cmd(0x16, 0x00C8); //MY=1 MX=0 MV=1 ML=0 BGR=1
        break;
    }

    display_set_area(0, 0, display_width-1, display_height-1);
}

void display_set_area(u16 x0, u16 y0, u16 x1, u16 y1)
{
    wr_cmd(0x03, (x0>>0)); //set x0
    wr_cmd(0x02, (x0>>8)); //set x0
    wr_cmd(0x05, (x1>>0)); //set x1
    wr_cmd(0x04, (x1>>8)); //set x1
    wr_cmd(0x07, (y0>>0)); //set y0
    wr_cmd(0x06, (y0>>8)); //set y0
    wr_cmd(0x09, (y1>>0)); //set y1
    wr_cmd(0x08, (y1>>8)); //set y1
}

// ----- Drawing Commands -----

void display_draw_start(void)
{
  CS_ENABLE();
  wr_spi(LCD_REGISTER);
  wr_spi(0x22);
  CS_DISABLE();

  CS_ENABLE();
  wr_spi(LCD_DATA);
}

void display_draw_pixel(u16 color)
{
  wr_spi(color>>8);
  wr_spi(color);
}

void display_draw_stop(void)
{
  CS_DISABLE();
}

void display_clear(u16 color)
{
  u16 size;

  display_set_area(0, 0, display_width-1, display_height-1);

  display_draw_start();
  for(size=(320UL*240UL/8UL); size!=0; size--)
  {
    display_draw_pixel(color); //1
    display_draw_pixel(color); //2
    display_draw_pixel(color); //3
    display_draw_pixel(color); //4
    display_draw_pixel(color); //5
    display_draw_pixel(color); //6
    display_draw_pixel(color); //7
    display_draw_pixel(color); //8
  }
  display_draw_stop();
}

static u16 fg_color = RGB(0xff,0xff,0xff);
static u16 bg_color = RGB(0x00,0x00,0x00);
static const prog_uint8_t *font_data;
static const u08 bitmask[] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };

static void draw_x1y1(u16 x, u16 y, const prog_uint8_t *ptr)
{
  for(u08 y=0;y<8;y++) {
    u08 bits = pgm_read_byte(ptr);
    for(u08 x=0;x<8;x++) {
      display_draw_pixel( bits & bitmask[x] ? fg_color : bg_color);
    }
    ptr++;
  }
}

static void draw_x2y1(u16 x, u16 y, const prog_uint8_t *ptr)
{
  for(u08 y=0;y<8;y++) {
    u08 bits = pgm_read_byte(ptr);
    for(u08 x=0;x<8;x++) {
      u16 col = bits & bitmask[x] ? fg_color : bg_color;
      display_draw_pixel(col);
      display_draw_pixel(col);
    }
    ptr++;
  }
}

static void draw_x1y2(u16 x, u16 y, const prog_uint8_t *ptr)
{
  for(u08 y=0;y<8;y++) {
    u08 bits = pgm_read_byte(ptr);
    for(u08 x=0;x<8;x++) {
      display_draw_pixel( bits & bitmask[x] ? fg_color : bg_color);
    }
    for(u08 x=0;x<8;x++) {
      display_draw_pixel( bits & bitmask[x] ? fg_color : bg_color);
    }
    ptr++;
  }
}

static void draw_x2y2(u16 x, u16 y, const prog_uint8_t *ptr)
{
  for(u08 y=0;y<8;y++) {
    u08 bits = pgm_read_byte(ptr);
    for(u08 x=0;x<8;x++) {
      u16 col = bits & bitmask[x] ? fg_color : bg_color;
      display_draw_pixel(col);
      display_draw_pixel(col);
    }
    for(u08 x=0;x<8;x++) {
      u16 col = bits & bitmask[x] ? fg_color : bg_color;
      display_draw_pixel(col);
      display_draw_pixel(col);
    }
    ptr++;
  }
}

typedef void (*text_func_t)(u16 x, u16 y, const prog_uint8_t *ptr);
static text_func_t text_func;
static text_func_t funcs[4] = { draw_x1y1, draw_x2y1, draw_x1y2, draw_x2y2 };
static u08 font_w;
static u08 font_h;

void display_set_color(u16 fg, u16 bg)
{
  fg_color = fg;
  bg_color = bg;
}

void display_set_font_data(const prog_uint8_t *data)
{
  font_data = data;
}

void display_set_font_scale(u08 x2, u08 y2)
{
  int off = 0;
  if(x2) off++;
  if(y2) off+=2;
  text_func = funcs[off];
  font_w = x2 ? 15 : 7;
  font_h = y2 ? 15 : 7;
}

void display_draw_char(u16 x,u16 y,u08 ch)
{
  u16 pos = ch << 3;
  const prog_uint8_t *ptr = font_data + pos;
  
  display_set_area(x,y,x+font_w,y+font_h);
  display_draw_start();
  text_func(x,y,ptr);
  display_draw_stop();
}

void display_draw_rect(u16 x, u16 y, u16 w, u16 h, u16 col)
{
  display_set_area(x,y,x+w-1,y+h-1);
  display_draw_start();
  for(u16 j=0;j<h;j++) {
    for(u16 i=0;i<w;i++) {
      display_draw_pixel( col );
    }
  }
  display_draw_stop();
}

void display_get_size(u08 *w, u08 *h)
{
  // fixed for now
  *w = 40;
  *h = 30;
}
