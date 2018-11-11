/*
 * coms.c
 *
 * Created: 10/8/2018 2:07:15 PM
 *  Author: arturo.arreola
 */ 

#include "coms.h"

void coms_initRs485(void) {
	//initialize USART as UART
	coms_uart0Init();
	
	/* Setup of RS485 ports:	
	-To transmit, REDE must be high
	-To receive, REDE must be low */
	LOW(REDE);
	OUTPUT(REDE);
} 

void coms_uart0Init(void)
{
	// Turn on the reception circuitry
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	// Use 8-bit character sizes
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
	// Enable USART RX interruption
	UCSR0B |= (1 << RXCIE0 );
	// Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRR0H = (BAUD_PRESCALE >> 8);
	// Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRR0L = BAUD_PRESCALE;
}

void coms_uart0Transmit( char data)
{
	while (( UCSR0A & (1<<UDRE0))==0){};	// Wait for empty transmit buffer
	UDR0 = data;							// Put data into buffer, sends the data */
}

char coms_uart0Receive( void )
{
	while ((UCSR0A & (1<<RXC0))==0){};		// Wait for data to be received
	return UDR0;
}

void coms_rs485Broadcast (char data)
{
	HIGH (REDE);	// To transmit, REDE must be high
	
	coms_uart0Transmit(data);
	_delay_ms(10);
	
	LOW (REDE);								// To receive, REDE must be low
}