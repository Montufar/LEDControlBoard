/*
 * PWM.c
 *
 * Created: 10/8/2018 2:04:59 PM
 *  Author: arturo.arreola
 */ 

#include "pwm.h"
#include "macros.h"
#define TRANSITION_DELAY 5

//global variables
int new_intensity = 0;
int old_intensity = 0;

char intensity_A_old = 0;
char intensity_B_old = 0;
char intensity_C_old = 0;

//prototypes
void pwm_initWaveLength1(void);
void pwm_initWaveLength2(void);
void pwm_initWaveLength3(void);
void pwm_setWaveLength1(char D);
void pwm_setWaveLength2(char D);
void pwm_setWaveLength3(char D);


void pwm_initLight(void) {
	//set output pins as high
	HIGH(PWM_1);
	HIGH(PWM_2);
	//HIGH(PWM_3);
	
	//set PWM pins as output
	OUTPUT(PWM_1);
	OUTPUT(PWM_2);
	//OUTPUT(PWM_3);
	
	//initialize timers as PWM
	pwm_initWaveLength1();
	pwm_initWaveLength2();
	//pwm_initWaveLength3();
}

void pwm_changeLightsIntensity (char light, char intensity)
{
	char D = 0;
	//intensity value must be a char value between 0 and 100
	if (intensity <= 100){
		//set the respective wavelength intensity
		switch (light) {
			case 1:
				if(intensity > intensity_A_old) {
					while(intensity_A_old < intensity) {
						intensity_A_old++;
						D = 255 - ((uint32_t) intensity_A_old * 255/100);
						pwm_setWaveLength1(D);
						_delay_ms(TRANSITION_DELAY);
					}
				} else if (intensity < intensity_A_old){
					while(intensity_A_old > intensity) {
						intensity_A_old--;
						D = 255 - ((uint32_t) intensity_A_old * 255/100);
						pwm_setWaveLength1(D);	
						_delay_ms(TRANSITION_DELAY);
					}
				}
				break;
			case 2:
				if(intensity > intensity_B_old) {
					while(intensity_B_old < intensity) {
						intensity_B_old++;
						D = 255 - ((uint32_t) intensity_B_old * 255/100);
						pwm_setWaveLength2(D);
						_delay_ms(TRANSITION_DELAY);
					}
				} else if (intensity < intensity_B_old){
					while(intensity_B_old > intensity) {
						intensity_B_old--;
						D = 255 - ((uint32_t) intensity_B_old * 255/100);
						pwm_setWaveLength2(D);
						_delay_ms(TRANSITION_DELAY);
					}
				}
				break;
			case 3:
				if(intensity > intensity_C_old) {
					while(intensity_C_old < intensity) {
						intensity_C_old++;
						D = 255 - ((uint32_t) intensity_C_old * 255/100);
						pwm_setWaveLength3(D);
						_delay_ms(TRANSITION_DELAY);
					}
				} else if (intensity < intensity_C_old){
					while(intensity_C_old > intensity) {
						intensity_C_old--;
						D = 255 - ((uint32_t) intensity_C_old * 255/100);
						pwm_setWaveLength3(D);
						_delay_ms(TRANSITION_DELAY);
					}
				}
				break;
			default:
				break;
		}
	}
}


void pwm_initWaveLength1 (void)
{
	//use OC2B for wavelength 1
	
	// Set duty cycle to the maximum - light off
	OCR2B = 255;
	// Compare Output Mode: Clear timer on Compare Match
	TCCR2A |= (1 << COM2B1);
	// Waveform Generation Mode 1: PWM, Phase Correct with top at 0xFF
	TCCR2A |= (1 << WGM20);
	// Clock Select: CLKIO/1
	TCCR2B |= (0 << CS22) | (0 << CS21) | (1 << CS20);
}

void pwm_initWaveLength2 (void)
{
	//use OC0B for wavelength 1
	
	// Set duty cycle to the maximum - light off
	OCR0B = 255;
	// Compare Output Mode: Clear timer on Compare Match
	TCCR0A |= (1 << COM0B1);
	// Waveform Generation Mode 1: PWM, Phase Correct with top at 0xFF						
	TCCR0A |= (1 << WGM00);
	// Clock Select: CLKIO/1
	TCCR0B |= (0 << CS22) | (0 << CS21) | (1 << CS20);
}

void pwm_initWaveLength3 (void)
{
	//use OC0A for wavelength 1
	
	// Set duty cycle to the maximum - light off
	OCR0A = 255;
	// Compare Output Mode: Clear timer on Compare Match
	TCCR0A |= (1 << COM0A1);
	// Waveform Generation Mode 1: PWM, Phase Correct with top at 0xFF
	TCCR0A |= (1 << WGM00);
	// Clock Select: CLKIO/1
	TCCR0B |= (0 << CS22) | (0 << CS21) | (1 << CS20);
}

void pwm_setWaveLength1 (char D)
{
	//Set duty cycle
	OCR2B = D;
}

void pwm_setWaveLength2 (char D)
{
	//Set duty cycle
	OCR0B = D;
}

void pwm_setWaveLength3 (char D)
{
	//Set duty cycle
	OCR0A = D;
}