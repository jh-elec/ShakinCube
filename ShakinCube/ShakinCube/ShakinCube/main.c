/*
 * ShakinCube.c
 *
 * Created: 14.01.2017 14:00:35
 * Author : Werkstatt
 */ 

#define F_CPU					128000

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#define LED_DDR					DDRB
#define LED_PORT				PORTB
#define LED_OFF					0xFF

#define CLEAR_PCINT0_FLAG		GIFR |= (1<<PCIF)

#define PINCHANGE0				1<<0

#define PULLUP_ON				PORTB |=  (1<<PB0)
#define PULLUP_OFF				PORTB &= ~(1<<PB0)

const char Numbers[6] PROGMEM = 
{
	0b11101111,
	0b11110111,
	0b11100111,
	0b11110011,
	0b11100011,
	0b11110001
};

volatile uint8_t Cnt 		= 0;
volatile uint8_t Interrupt 	= 0;

void ShakeIt(void);
void Init_INT0(void);
void Sleep(void);

int main(void)
{
	LED_DDR	|= ((1<<PB4) | (1<<PB3) | (1<<PB2) | (1<<PB1));
	LED_DDR	&= ~(1<<PB0);	
	PULLUP_ON;

	LED_PORT	= LED_OFF;

	Init_INT0();
	
	Sleep();
	
    while (1) 
    {
		if ((Interrupt & PINCHANGE0) == PINCHANGE0)
		{
			Interrupt &= ~0x01;
			ShakeIt();
		}
    }
}

void Init_INT0(void)
{
	PCMSK = (1<<PCINT0); // Choose Pin Change Interrupt
	GIMSK = (1<<PCIE); // Enable Pin Change Interrupt
	sei();
}

ISR(PCINT0_vect)
{
	Interrupt |= PINCHANGE0; // Save, Pin Change Interrupt was triggered 
}

void ShakeIt(void)
{
	uint8_t Result = 0x00;
	
	for (Cnt = 0 ; Cnt < 10 ; Cnt++)
	{
		Result = rand() % 6;
		LED_PORT = pgm_read_byte(&Numbers[Result]);
		_delay_ms(45);
	}
	
	_delay_ms(1500);
	
	for (Cnt = 0 ; Cnt < 6 ; Cnt++)
	{
		LED_PORT = LED_OFF;
		_delay_ms(150);
		LED_PORT =	pgm_read_byte(&Numbers[Result]);
		_delay_ms(150);
	}
	
	LED_PORT = LED_OFF;

	Sleep();
}

void Sleep(void)
{
	CLEAR_PCINT0_FLAG;

	LED_DDR	&= ~((1<<PB4) | (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0));
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();
	sleep_disable();
	
	LED_DDR	|= ((1<<PB4) | (1<<PB3) | (1<<PB2) | (1<<PB1));
	LED_DDR	&= ~(1<<PB0);
	PULLUP_ON;	
}
