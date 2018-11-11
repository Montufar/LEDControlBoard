/*
 * coms.h
 *
 * Created: 10/8/2018 2:07:23 PM
 *  Author: arturo.arreola
 */ 


#ifndef COMS_H_
#define COMS_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "macros.h"

/* Pin definitions */
#define REDE	D,2

//definition of all the LED addresses
#define HEX_ID_0 0x00		//when a message is sent to this address, all control boards act to the command
#define HEX_ID_1 0x01		//lamp 1
#define HEX_ID_2 0x02		//lamp 2
#define HEX_ID_3 0x03		//lamp 3
#define HEX_ID_4 0x04		//lamp 4
#define HEX_ID_5 0x05		//lamp 5
#define HEX_ID_6 0x06		//lamp 6
#define HEX_ID_7 0x07		//lamp 7

#define HEX_ID HEX_ID_4		//CHANGE HERE THE ADDRESS OF EACH PCB

/* Messages */
#define HEX_MSG_HANDSHAKE			0x01
#define HEX_MSG_GET_TEMPERATURE		0x02
#define HEX_MSG_GET_HUMIDITY		0x03
#define HEX_MSG_GET_WAVELENGHT		0x04
#define HEX_MSG_SET_BLUE_CURRENT	0x05
#define HEX_MSG_SET_RED_CURRENT		0x06
#define HEX_MSG_GETPOSITION			0x07
#define HEX_MSG_ACK					0x08

typedef union coms_message_UNION {
	uint16_t W;
	struct {
		uint16_t ID			: 4;
		uint16_t CMD		: 4;
		uint16_t VAL		: 8;
	} F;
	struct {
		uint16_t firstHalf	: 8;
		uint16_t secondHalf : 8;
	} B;
} coms_message_U;

void coms_uart0Init(void);				// USART0 communicates through an RS-485 interface. TO DO: change the name of the function
void coms_uart0Transmit(char data);	// To transmit, REDE must be high
char coms_uart0Receive(void);			// To receive, REDE must be low
void coms_rs485Broadcast (char data);

void coms_initRs485(void);



#endif /* COMS_H_ */