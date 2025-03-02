
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
	uint16_t ticks;
	
	// set timer 1 to measure 38ms timeout 
	TCCR1A |= (1<<WGM12);	// start timer in CTC mode
	TCCR1B |= (1<<CS12);	// pre-scale to 256
	OCR1A = 2374;			// count 38ms
	
	// check for high pulse on the echo pin after trigger pulse is released
	// will wait for 38 ms, if 38ms pass, echo pin is faulty 
	while(!(TIFR1 & (1<<OCF1A))) {
		if(!(HCSR04_PIN & (1<<HCSR04_ECHO_PIN))) {
			continue; // ECHO pin is LOW continue
		} else {
			TIFR1 &= ~(1<<OCF1A); 
			break; // HIGH on ECHO pin detected, break loop
		}
	}
	
	// check if OCF bit was cleared successfully
	if(TIFR1 & (1<<OCF1A))	 {
		return -1; // failed to activate echo pin
	} 	
	
	// wait for FALLING EDGE on the ECHO pin - polling 
	while(!(TIFR1 & (1<<OCF1A))) {
		if(!(HCSR04_PIN & (1<<HCSR04_ECHO_PIN))) { // falling edge detected
			ticks = TCNT1;
			TCNT1 = 0x00;
			TCCR1A = 0x00; // stop timer 
			TIFR1 &= ~(1<<OCF1A);
			break;
		} else {
			continue;
		}
	}
	
	// check if the OCF1A flag is still 0, meaning timeout on the ECHO pin
	// if 38ms pass without detecting a LOW on the ECHO pin, then there is no obstacle on that side
	if(TIFR1 & (1<<OCF1A))	 {
		return -2; // no obstacle in range
	} 
	
	return ticks;
	
}


void onboard_led_init() {
	DDRB |= (1<<PB7);
}

int main(void)
{
	hcsro4_init();
    usart_init();
	onboard_led_init();
	
	char m[] = "Hello USONIC\n";
	char d_buff[20];
	uint16_t r = 0;
	
    while (1) 
    {
		hcsro4_trigger();
		r = hcsro4_get_pulse_width();
		
		if(r == -1) {
			debug_via_usart("Sensor error");
		} else if(r == -2) {
			debug_via_usart("No obstacle in range");
		} else {
			// calculate the distance 
			uint16_t pulse_width = r * (16E-6);
			uint16_t distance = (pulse_width * 340) * 100; // distance in centimeters
			sprintf(d_buff, "%d\n", distance);
			debug_via_usart(d_buff);
			_delay_ms(10);
		}
		
    }
}

