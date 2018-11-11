/*
 * pwm.h
 *
 * Created: 10/8/2018 2:05:16 PM
 *  Author: arturo.arreola
 */ 


#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "macros.h"

#define PWM_1	D,3
#define PWM_2	D,5
#define PWM_3	D,6

#define MAX_INTENSITY 0
#define MIN_INTENSITY 255

#define HEX_COLORS_RED_BLUE			0
#define HEX_COLORS_WHITE			1
#define HEX_COLORS_RED_BLUE_WHITE	2

#define HEX_COLORS HEX_COLORS_RED_BLUE		//CHANGE HERE THE COLOR DEPENDING OF EACH LED PCB


/*Prototypes*/
void pwm_initLight (void);
void pwm_changeLightsIntensity (char light, char intensity);





#endif /* PWM_H_ */