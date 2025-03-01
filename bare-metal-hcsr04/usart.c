#include "usart.h"

void usart_init()
{
	// set 9600 baud rate
	uint32_t ubrr = (F_CPU/16UL)/BAUD - 1;
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = ubrr;
	
	// enable transmitter and receiver
	UCSR0B |= (1<<RXEN0);
	UCSR0B |= (1<<TXEN0);
	
	// set the frame format 8-bit, 2-stop bit
	UCSR0C = (1<<USBS0) | (3 << UCSZ00);
	
}

void usart_transmit(unsigned char c)
{
	// wait for empty transmit buffer
	while ( !(UCSR0A & (1 <<UDRE0)));
	
	// put data into buffer
	UDR0 = c;
}

void debug_via_usart(char* s)
{
	for(int j =0; j<strlen(s); j++)
	{
		usart_transmit(s[j]);
	}
	
}