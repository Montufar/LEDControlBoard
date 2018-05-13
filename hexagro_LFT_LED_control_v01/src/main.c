/*
 * hexagro_LFT_LED_control_v01
 *
 * main.c
 *
 * Created: 16/03/2017 21:28:46
 * Author: Arturo
 * For: Hexagro Urban Farming S.R.L. for Benefit
 */

//definition of all the LED addresses
 #define HEX_ID_0 0x00		//when a message is sent to this address, all control boards act to the command
 #define HEX_ID_1 0x01		//lamp 1
 #define HEX_ID_2 0x02		//lamp 2
 #define HEX_ID_3 0x03		//lamp 3
 #define HEX_ID_4 0x04		//lamp 4
 #define HEX_ID_5 0x05		//lamp 5
 #define HEX_ID_6 0x06		//lamp 6
 #define HEX_ID_7 0x07		//lamp 7

#define MAX_INTENSITY 0
#define MIN_INTENSITY 255

#define HEX_ID HEX_ID_1		//CHANGE HERE THE ADDRESS OF EACH PCB

#define F_CPU 8000000u								//oscillation frequency
#define FOSC F_CPU									//oscillation frequency definition for delay functions
#define USART_BAUDRATE 9600							//RS485 communication frequency
#define BAUD_PRESCALE (FOSC/16/USART_BAUDRATE-1)	//UART driver pre-scaler

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <asf.h>

#include <string.h>
#include <stdio.h>
#include <assert.h>

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

/* Pin definitions */
#define PWM_1	D,3
#define PWM_2	D,5
#define PWM_3	D,6
#define REDE	D,2

/* Messages */
#define HEX_MSG_HANDSHAKE			0x01
#define HEX_MSG_GET_TEMPERATURE		0x02
#define HEX_MSG_GET_HUMIDITY		0x03
#define HEX_MSG_GET_WAVELENGHT		0x04
#define HEX_MSG_SET_BLUE_CURRENT	0x05
#define HEX_MSG_SET_RED_CURRENT		0x06
#define HEX_MSG_GETPOSITION			0x07
#define HEX_MSG_ACK					0x08

/*Prototypes*/
void PWM_1_set(int D);
void PWM_2_set(int D);
void PWM_3_set(int D);
void PWM_1_change(int D);
void PWM_2_change(int D);
void PWM_3_change(int D);
void USART0_Init(void);				// USART0 communicates through an RS-485 interface. TO DO: change the name of the function
void USART0_Transmit(char data);	// To transmit, REDE must be high
char USART0_Receive(void);			// To receive, REDE must be low
void RS485_Broadcast (char *message_string);
void RS485_ResetMessage(void);
char** str_split(char* a_str, const char a_delim);

char received_message[100];
char received_message_counter = 0;
char received_message_ID = 0;
char received_message_function_1[10];
char received_message_function_2[10];
char received_message_function_3[10];
char received_message_value = 0;

char output_message[100];

char RS485_message_status = 0; // 0 is reseted, 1 is 1 received, 2 is 2 received, 3 is 3 received or complete
char RS485_timeout_state = 0;
int RS485_timeout_counter = 0;

char** tokens;

int change_light_intensity (char intensity);
int new_intensity = 0;
int old_intensity = 0;

char flag_1 = 1;

int main (void)
{
	HIGH(PWM_1);
	HIGH(PWM_2);
	HIGH(PWM_3);
	
	OUTPUT(PWM_1);
	OUTPUT(PWM_2);	
	OUTPUT(PWM_3);	
	
	PWM_1_set(255);
	PWM_2_set(255);
	PWM_3_set(255);
	
	change_light_intensity(0.0);
	
	USART0_Init();
	sei();
	
	while(1)
	{
		if(flag_1)
		{
			for (char i = 0; i <= 100; i++)
			{
				change_light_intensity(i);
				_delay_ms(10);
			}
			for (char i = 100; i >= 0; i--)
			{
				change_light_intensity(i);
				_delay_ms(10);
			}
		}
		_delay_ms(1);
	}
}

int change_light_intensity (char intensity)
{
	if ((intensity >= 0)&&(intensity <= 100)){
		int D = 255 - (intensity*255/100);
		PWM_2_change(D);
		PWM_3_change(D);
		return 0;
	}
	else
		return -1;
}

/* Functions */
void PWM_1_set (int D)
{
	/* OC2B */
	OCR2B = D;											//Set duty cycle
	TCCR2A |= (1 << COM2B1);							// Compare Output Mode: Clear OC2B on Compare Match.
	TCCR2A |= (1 << WGM20);								// Waveform Generation Mode 1: PWM, Phase Correct with top at 0xFF
	TCCR2B |= (0 << CS22) | (0 << CS21) | (1 << CS20);	// Clock Select: CLKIO/1
	//OCR2B_value = D;
}

void PWM_2_set (int D)
{
	/* OC0B */
	OCR0B = D;
	TCCR0A |= (1 << COM0B1);							// Compare Output Mode: Clear OC2B on Compare Match.
	TCCR0A |= (1 << WGM00);								// Waveform Generation Mode 1: PWM, Phase Correct with top at 0xFF
	TCCR0B |= (0 << CS22) | (0 << CS21) | (1 << CS20);	// Clock Select: CLKIO/1
	//OCR0B_value = D;
}

void PWM_3_set (int D)
{
	/* OC0A */
	OCR0A = D;
	TCCR0A |= (1 << COM0A1);							// Compare Output Mode: Clear OC2B on Compare Match.
	TCCR0A |= (1 << WGM00);								// Waveform Generation Mode 1: PWM, Phase Correct with top at 0xFF
	TCCR0B |= (0 << CS22) | (0 << CS21) | (1 << CS20);	// Clock Select: CLKIO/1
	//OCR0A_value = D;
}

void PWM_1_change (int D)
{
	OCR2B = D;											//Set duty cycle
	//OCR2B_value = D;
}

void PWM_2_change (int D)
{
	OCR0B = D;											//Set duty cycle
	//OCR0B_value = D;
}

void PWM_3_change (int D)
{
	OCR0A = D;											//Set duty cycle
	//OCR0B_value = D;
}

void USART0_Init(void)
{
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);   // Turn on the reception circuitry
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes
	UCSR0B |= (1 << RXCIE0 );				 // Enable USART RX interruption
	UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	
	/* Setup of RS485 ports:	
		-To transmit, REDE must be high
		-To receive, REDE must be low */
	LOW(REDE);
	OUTPUT(REDE);
}

void USART0_Transmit( char data)
{
	while (( UCSR0A & (1<<UDRE0))==0){};	// Wait for empty transmit buffer
	UDR0 = data;							// Put data into buffer, sends the data */
}

char USART0_Receive( void )
{
	while ((UCSR0A & (1<<RXC0))==0){};		// Wait for data to be received
	return UDR0;
}

ISR ( USART_RX_vect )
{
	received_message[received_message_counter] = UDR0;
	if (received_message[received_message_counter] =='\r')
	{
		received_message[received_message_counter] = NULL;
		received_message_counter = 0;
		
		tokens = str_split(received_message, ',');
		if (tokens)
		{
			received_message_ID = atoi(*(tokens + 0));
			free(*(tokens + 0));
			strncpy(received_message_function_1, *(tokens + 1), 10);
			strncpy(received_message_function_2, received_message_function_1, 10);
			strncpy(received_message_function_3, received_message_function_2, 10);
			free(*(tokens + 1));
			received_message_value = atoi(*(tokens + 2));
			free(*(tokens + 2));
			free(tokens);
			
			// ID processing
			if((received_message_ID == HEX_ID)||(received_message_ID == HEX_ID_0))
			{
				//RS485_Broadcast("message is mine\n");
				// Function processing
				if (strcmp(received_message_function_1, "light_on")==0)
				{	
					flag_1 = 0;
					// Value processing
					itoa(received_message_value,output_message,10);
					//RS485_Broadcast("turning light on to ");
					//RS485_Broadcast(output_message);
					//RS485_Broadcast("\n");
					new_intensity = received_message_value;
					
					if (new_intensity > old_intensity)
					{
						for (char i = old_intensity; i <= new_intensity; i++)
						{
							change_light_intensity(i);
							_delay_ms(10);
						}
						//change_light_intensity(new_intensity);
					}
					else if (new_intensity < old_intensity)
					{
						for (char i = old_intensity; i >= new_intensity; i--)
						{
							change_light_intensity(i);
							_delay_ms(10);
						}
					}					
					old_intensity = new_intensity;
					
				} 
				else if (strcmp(received_message_function_2, "light_off")==0) 
				{
					flag_1 = 0;
					//RS485_Broadcast("turning light off\n");
					new_intensity = 0;
					for (char i = old_intensity; i >= new_intensity; i--)
					{
						change_light_intensity(i);
						_delay_ms(10);
					}
					old_intensity = new_intensity;
				} 
				else if (strcmp(received_message_function_3, "light_fls")==0)
				{
					//RS485_Broadcast("flashing light\n");
					flag_1 = 1;
				} 
				else 
				{
					//RS485_Broadcast("message unknown\n");
				}
				//RS485_Broadcast("1,ACK,0\r");
			}
			else
			{
				//RS485_Broadcast("message is not mine\n");
			}	
		}
	}
	else
	{
		received_message_counter++;             // notify main of receipt of data.
	}
}

void RS485_Broadcast (char *message_string)
{
	HIGH (REDE);							// To transmit, REDE must be high
	
	uint8_t c;
	while ((c=*message_string++)){
		USART0_Transmit(c);
	}
	
	_delay_ms(10);
	
	LOW (REDE);								// To receive, REDE must be low
}

void RS485_ResetMessage(void)
{
	RS485_message_status = 0;
	RS485_timeout_counter = 0;
	RS485_timeout_state = 0;
	for (int i = 0; i < 100; i++){
		received_message[i] = 0;
	}
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp){
        if (a_delim == *tmp){
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result){
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token){
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}


/************************************************************************/
/*								Code end                                */
/************************************************************************/