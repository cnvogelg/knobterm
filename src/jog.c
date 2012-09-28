/*
 * jog.c - handle jog wheel
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
 *
 * Based on
 *   http://www.mikrocontroller.net/articles/Drehgeber
 *   http://www.mikrocontroller.net/articles/Entprellung
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

#include "jog.h"

#include <avr/io.h>
#include <avr/interrupt.h>

/*
	using a Panasonic EVEP Jog Wheel with Button
	(pollin.de: Encoder PANASONIC EVEQDBRL416B, Nr: 240 313) 

	Jog Wheel Wiring:
	PINC0	= Jog Button
	PINC1	= Jog Phase A
	PINC2	= Jog Phase B
*/

#define BUTTON      (PINC & 4)
#define PHASE_A     (PINC & 1)
#define PHASE_B     (PINC & 2)
#define BUTTON_MASK 4

// jog 
volatile int8_t enc_delta;          // -128 ... 127
static int8_t last;

// button
volatile uint8_t key_state;
volatile uint8_t key_press;
volatile uint8_t key_rpt;

void jog_init( void )
{
    int8_t new;

    new = 0;
    if( PHASE_A )
        new = 3;
    if( PHASE_B )
        new ^= 1;                   // convert gray to binary
    last = new;                   // power on state
    enc_delta = 0;

   // setup timer
#ifdef TCCR0A
    TCCR0A = 1<<WGM01;
    TCCR0B = 1<<CS01 | 1<<CS00;
#else
    TCCR0 = 1<<WGM01 | 1<<CS01 | 1<<CS00;     // CTC, XTAL / 64
#endif
   
  // how many ticks n until 1ms is reached?
  //
  // t_tick * n = 1 ms
  //
  // -> n = 1 * F_CPU / ( prescaler * 1000 )
  // -> compare val m = n -1
#define TIMER0_COMPARE_VAL  ((1 * F_CPU) / (64 * 1000)) - 1

#ifdef OCR0A
    OCR0A = 249; //TIMER0_COMPARE_VAL;
#else
    OCR0 = TIMER0_COMPARE_VAL;
#endif

#ifdef TIMSK0
    TIMSK0 |= 1<<OCIE0A;
#else
    TIMSK |= 1<<OCIE0;
#endif
}

#ifdef TIMER0_COMPA_vect
ISR( TIMER0_COMPA_vect )
#else
ISR( TIMER0_COMP_vect )
#endif
{
    int8_t new, diff;
    static uint8_t pre_scale;
 
    new = 0;
    if( PHASE_A )
        new = 3;
    if( PHASE_B )
        new ^= 1;                   // convert gray to binary
    diff = last - new;                // difference last - new
    if( diff & 1 ){               // bit 0 = value (1)
        last = new;                 // store new as next last
         enc_delta += (diff & 2) - 1;        // bit 1 = direction (+/-)
    }

    // every 10 ms
    pre_scale++;
    if(pre_scale == 10) {
        pre_scale = 0;
    
        // button stuff
        static uint8_t ct0, ct1, rpt;
        uint8_t i = (key_state ^ ~BUTTON) & BUTTON_MASK;
        ct0 = ~(ct0 & i);
        ct1 = ct0 ^ (ct1 & i);
        i &= ct0 & ct1;
        key_state ^= i;
        key_press |= key_state & i;
    
        // key long press and repeat
        if(key_state == 0) {
            rpt = 50; // after 500 ms
        }
        if( --rpt == 0 ) {
            rpt = 20; // next repeat after 200 ms
            key_rpt |= key_state;
        }
    }
}

int8_t jog_read( void )         // read two step encoders
{
    int8_t val;
 
    cli();
    val = enc_delta;
    enc_delta = val & 1;
    sei();
    return val >> 1;                   // counts since last call
}

uint8_t jog_button_press( void )
{
    cli();
    uint8_t key = key_press;
    key_press ^= key;
    sei();
    return key;
}

uint8_t jog_button_repeat( void )
{
    cli();
    uint8_t key = key_rpt;
    key_rpt = 0;
    sei();
    return key;
}
