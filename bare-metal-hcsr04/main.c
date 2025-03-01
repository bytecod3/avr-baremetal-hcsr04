
#include <avr/io.h>
#define F_CPU 16000000UL

#include "usart.h"
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>

#define HCSR04_DDR			DDRK // port assumes using ATMEGA2560
#define HCSR04_PIN			PINK
#define HCSR04_TRIGGER_PIN	PK6
#define HCSR04_ECHO_PIN		PK7
#define TRIG_PULSE_LENGTH	10 // trigger length in us

volatile int c = 0;
ISR(TIMER1_COMPA_vect) {
	PORTB ^= (1<<PB7);
}

void hcsro4_init() {
	DDRK |= (1<<PK0); // trigger pin output 
}

void hcsro4_trigger() {
	HCSR04_DDR |= (1<<HCSR04_TRIGGER_PIN); 
	_delay_us(TRIG_PULSE_LENGTH);
	HCSR04_DDR &= ~(1<<HCSR04_TRIGGER_PIN);
}

uint16_t hcsro4_get_pulse_width() {
	// measure pulse duration received from the echo pin
	uint32_t i, result;
	
	// set timeout of waiting for the pin
	for(i=0; i<60000; i++) {
		if(!(PINK & (1<<PK1))) {
			continue; // line is low
		} else {
			break; // detect a transition to HIGH
		}
	}
	
	// check the echo pin for a given amount of time 
	
	// if timeout detected, return ERROR
	if(i==60000) {
		return -1;
	}
	
	// set up timer 1
	TCCR1A = 0x00;
	TCCR1B |= (1<<CS11); // pre-scale by 8 (T=500nS)
	TCNT1 = 0x00;
}


void timer3_check_timeout() {
	TCCR3A = 0x00;
	TCCR3B |= (1<<CS30); // no pre-scaling
	uint16_t v = 608;
	TCNT3 = 0x00;
	
	if(TCNT3 == v) {
		// 38 ms reached, reset counter
		TCNT3 = 0x00;
		debug_via_usart("38MS");
	}
}

void onboard_led_init() {
	DDRB |= (1<<PB7);
}


void timer1_CTC_mode() {
	TCCR1B |= (1<<WGM12); // CTC mode 
	TCCR1B |= (1<<CS12); // set pre-scaler to 256
	OCR1A = 31249; // compare value for 500ms (see docs for calculation)
	TIMSK1 |= (1<<OCIE1A); // enable interrupt
	//TCCR1A |= (1<<COM1A0); // toggle output on compare match
	sei();
}

int main(void)
{
    usart_init();
	char m[] = "Hello USONIC\n";
	
	onboard_led_init();
	//DDRB |= (1<<PB5);
	timer1_CTC_mode();
	
    while (1) 
    {
		
    }
}

