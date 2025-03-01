

#ifndef USART_H_
#define USART_H_

#define F_CPU 16000000UL

#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600

void usart_init();
void usart_transmit(unsigned char);
void debug_via_usart(char*);
void usart_receive();

#endif /* USART_H_ */