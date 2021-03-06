/*
* uart.c - serial hw routines
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

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"

#ifdef UBRR0H

// for atmeg644
#define UBRRH  UBRR0H
#define UBRRL  UBRR0L
#define UCSRA  UCSR0A
#define UCSRB  UCSR0B
#define UCSRC  UCSR0C
#define UDRE   UDRE0
#define UDR    UDR0

#define RXC    RXC0
#define TXC    TXC0
#define DOR    DOR0
#define PE     UPE0

#define RXEN   RXEN0
#define TXEN   TXEN0
#define UCSZ0  UCSZ00
#define UCSZ1  UCSZ01
#define RXCIE  RXCIE0

#endif

// calc ubbr from baud rate
#define UART_UBRR   F_CPU/16/UART_BAUD-1

#define UART_RX_BUF_SIZE 80
static volatile u08 uart_rx_buf[UART_RX_BUF_SIZE];
static volatile u08 uart_rx_start = 0;
static volatile u08 uart_rx_end = 0;
static volatile u08 uart_rx_size = 0;

void uart_init(void) 
{
    // baud rate
    UBRRH = (u08)((UART_UBRR)>>8);
    UBRRL = (u08)((UART_UBRR)&0xff);

    UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);   // enable tranceiver and transmitter
    UCSRC = _BV(UCSZ0) | _BV(UCSZ1); // 8 bit, 1 stop, no parity, asynch. mode

    uart_rx_start = 0;
    uart_rx_end = 0;
    uart_rx_size = 0;
}

// receiver interrupt
#ifdef USART_RXC_vect
ISR(USART_RXC_vect)
#else
ISR(USART_RX_vect)
#endif
{
    u08 data = UDR;
    uart_rx_buf[uart_rx_end] = data;

    uart_rx_end++;
    if(uart_rx_end == UART_RX_BUF_SIZE)
        uart_rx_end = 0;

    uart_rx_size++;
}

u08 uart_read_data_available(void)
{
    return uart_rx_start != uart_rx_end;
}

u08 uart_read(void)
{
    // read for buffe to be filled
    while(uart_rx_start==uart_rx_end);

    // read buffer
    cli();

    u08 data = uart_rx_buf[uart_rx_start];

    uart_rx_start++;
    if(uart_rx_start == UART_RX_BUF_SIZE)
        uart_rx_start = 0;

    uart_rx_size--;  

    sei();
    return data;
}

void uart_send(u08 data)
{
    // wait for transmitter
    while(!( UCSRA & _BV(UDRE)));
    // send byte
    UDR = data;
}
