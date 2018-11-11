/*
 * macros.h
 *
 * Created: 10/8/2018 2:02:31 PM
 *  Author: arturo.arreola
 */ 

#ifndef MACROS_H_
#define MACROS_H_

#ifndef F_CPU
#define F_CPU 8000000u								//oscillation frequency
#endif //F_CPU
#include <avr/io.h>
#include <util/delay.h>

#define FOSC F_CPU									//oscillation frequency definition for delay functions
#define USART_BAUDRATE 9600							//RS485 communication frequency
#define BAUD_PRESCALE (FOSC/16/USART_BAUDRATE-1)	//UART driver pre-scaler

#ifdef _BV
#define _SET(type,name,bit)			type ## name  |= _BV(bit)
#define _CLEAR(type,name,bit)       type ## name  &= ~ _BV(bit)
#define _TOGGLE(type,name,bit)      type ## name  ^= _BV(bit)
#define _GET(type,name,bit)         ((type ## name >> bit) &  1)
#define _PUT(type,name,bit,value)   type ## name = ( type ## name & ( ~ _BV(bit)) ) | ( ( 1 & (unsigned char)value ) << bit )

//these macros are used by end user
#define OUTPUT(pin)		_SET(DDR,pin)
#define INPUT(pin)      _CLEAR(DDR,pin)
#define HIGH(pin)       _SET(PORT,pin)
#define LOW(pin)        _CLEAR(PORT,pin)
#define TOGGLE(pin)     _TOGGLE(PORT,pin)
#define READ(pin)       _GET(PIN,pin)

//#define ledON(pin)		_CLEAR(PORT,pin)
//#define ledOFF(pin)		_SET(PORT,pin)
#endif //_BV

#define HI 1
#define LO 0

#endif /* MACROS_H_ */