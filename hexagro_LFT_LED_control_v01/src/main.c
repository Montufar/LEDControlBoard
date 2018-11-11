/*
 * hexagro_LFT_LED_control_v01
 *
 * main.c
 *
 * Created: 16/03/2017 21:28:46
 * Author: Arturo
 * For: Hexagro Urban Farming S.R.L. for Benefit
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "macros.h"
#include "pwm.h"
#include "coms.h"

uint32_t os_counter = 0;
uint8_t os_stateMachine = 0;
uint8_t os_ledBlinkCount = 0;
uint8_t coms_messageReceivedCounter = 0;
uint8_t coms_messageReceivedTimeout = 0;
uint8_t coms_idCmdMessage = 0;
uint8_t coms_valueMessage = 0;
coms_message_U coms_receivedMessage = { 0 };

int main (void)
{
	//initialize PWM outputs
	pwm_initLight();
	
	//initialize communication
	coms_initRs485();
	
	//enable interrupts
	sei();
	
	while(1)
	{
		//TODO: change counter to a timer counter instead of the delay...
		//TODO: dimm light change
		
		//test blink
		if(os_stateMachine == 0u) {
			if((os_counter % 1u) == 0u) {
				os_ledBlinkCount++;
				if(os_ledBlinkCount >= 300u) {
					os_ledBlinkCount = 0u;
				}
			}if (os_ledBlinkCount <= 100u)
			{
				pwm_changeLightsIntensity(1u, os_ledBlinkCount);
				pwm_changeLightsIntensity(2u, os_ledBlinkCount);
				//pwm_changeLightsIntensity(3, os_ledBlinkCount);
			} else if ((os_ledBlinkCount > 100u) && (os_ledBlinkCount <= 200u)){
				pwm_changeLightsIntensity(1u, 200u - os_ledBlinkCount);
				pwm_changeLightsIntensity(2u, 200u - os_ledBlinkCount);
				//pwm_changeLightsIntensity(3, 200 - os_ledBlinkCount);
			}
		}
		
		//handle communication timeout after 100 ms of last message received
		if(coms_messageReceivedCounter > 0u) {
			coms_messageReceivedTimeout++;
			if(coms_messageReceivedTimeout >= 100u) {
				coms_messageReceivedTimeout = 0u;
				coms_messageReceivedCounter = 0u;
				coms_idCmdMessage = 0u;
				coms_valueMessage = 0u;
			}
		}
		
		//if the buffer is full, process data received
		if(coms_messageReceivedCounter == 2) {
			coms_messageReceivedCounter = 0;
			coms_receivedMessage.B.firstHalf = coms_idCmdMessage;
			coms_receivedMessage.B.secondHalf = coms_valueMessage;
			
			if((coms_receivedMessage.F.ID == HEX_ID) || (coms_receivedMessage.F.ID == HEX_ID_0)) {
				switch (coms_receivedMessage.F.CMD) {
					
					case 0x00u:
						//blink
						os_stateMachine = 0;
						break;
						
					case 0x01u:
						//set red intensity
						if(coms_receivedMessage.F.VAL <= 100u) {
							pwm_changeLightsIntensity(1, (uint8_t) coms_receivedMessage.F.VAL);
						}
						break;
						
					case 0x02u:
						//set red intensity
						if(coms_receivedMessage.F.VAL <= 100u) {
							pwm_changeLightsIntensity(2, (uint8_t) coms_receivedMessage.F.VAL);
						}
						break;
						
					case 0x03u:
						//set red intensity
						if(coms_receivedMessage.F.VAL <= 100u) {
							pwm_changeLightsIntensity(3, (uint8_t) coms_receivedMessage.F.VAL);
						}
						break;

					default:
						break;		
				}
			}
		}
		
		//tick every 10 ms and sleep
		os_counter++;
		_delay_ms(1);
		//clear the counter every second
		if(os_counter >= 1000) {
			os_counter = 0;
		}
	}
}

//Interrupt service routine for RX message received
ISR ( USART_RX_vect )
{
	os_stateMachine = 1;
	//disable interrupts
	//cli();
	
	//if this is the first message:
	if (coms_messageReceivedCounter == 0) {
		coms_idCmdMessage = UDR0;
		coms_messageReceivedCounter++;
	} 
	//if this is the second message:
	else if (coms_messageReceivedCounter == 1) {
		coms_valueMessage = UDR0;
		coms_messageReceivedCounter++;
	}
	//error 
	else {
		//error;
		coms_idCmdMessage = 0;
		coms_valueMessage = 0;
	}
	
	//enable interrupts
	//sei();
	//pwm_changeLightsIntensity(1, 50);
}

/************************************************************************/
/*								Code end                                */
/************************************************************************/