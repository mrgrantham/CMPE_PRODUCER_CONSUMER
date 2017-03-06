/*
 * UARTdriver.cpp
 *
 *  Created on: Mar 5, 2017
 *      Author: James
 */

#include "UARTdriver.hpp"
#include "LPC17xx.h"
#include <stdio.h>


UARTMode UARTdriver::uartMode = NO_MODE;


UARTdriver::UARTdriver(uint8_t priority) : scheduler_task("task", 2000, priority)
{
	/* Nothing to init */
	setRunDuration(200);
}

bool UARTdriver::init(void)
{
	LPC_SC->PCONP |= (1 << 24); // turn on the UART

	// set clock divider

	// for uart2
	LPC_SC->PCLKSEL1 &= ~(3 << 16); // clear the 2 bits
	LPC_SC->PCLKSEL1 |= (1 << 16); // set the 1 for clock divider

	// for uart3
	LPC_SC->PCLKSEL1 &= ~(3 << 18); // clear the 2 bits
	LPC_SC->PCLKSEL1 |= (1 << 18); // set the 1 for clock divider

	// 21:20 P0.10 TXD2
	// 23:22 P0.11 RXD2

	//	1:0 P0.0 TXD3
	//	3:2 P0.1 RXD3

	// set baud rate divider

	LPC_UART2->LCR |= ( 1 << 7 );
	LPC_UART3->LCR |= ( 1 << 7 );


	const uint32_t  baud_rate = 9600;
	const uint16_t div16 = sys_get_cpu_clock() / (16 * baud_rate);

	LPC_UART2->DLL = (div16 + 0.5); // lower byte
	LPC_UART2->DLM = 0; // upper byte

	LPC_UART3->DLL = (div16 + 0.5); // lower byte
	LPC_UART3->DLM = 0; // upper byte

	LPC_PINCON->PINSEL4 &= ~(3 << 16);
	LPC_PINCON->PINSEL4 |= (2 << 16);   // TXD2

	LPC_PINCON->PINSEL4 &= ~(3 << 18);
	LPC_PINCON->PINSEL4 |= (2 << 18);   // RXD2

	LPC_PINCON->PINSEL9 |= (3 << 24);   // TXD3
	LPC_PINCON->PINSEL9 |= (3 << 26);   // RXD3

	// set to 8-bit length
	LPC_UART2->LCR = 3;
	LPC_UART3->LCR = 3;

	return true;
}

void UARTdriver::uart2_send(char data) {
	while (!(LPC_UART2->LSR & (1 << 6))) {
		// add sleep function
		vTaskDelay(10);
	}
	LPC_UART2->THR = data; //sending byte
}

char UARTdriver::uart2_receive() {
	//check status register and then read
	while (!(LPC_UART2->LSR & (1 << 5))) { // checks for 0 to indicate valid data present
		vTaskDelay(1);
	}
	uint8_t data = LPC_UART2->RBR;

	return data;
}

void UARTdriver::uart3_send(char data) {
	while (!(LPC_UART3->LSR & (1 << 6))) {
		// add sleep function
		vTaskDelay(1);
	}
	LPC_UART3->THR = data; //sending byte
}

char UARTdriver::uart3_receive() {
	//check status register and then read
	while (!(LPC_UART3->LSR & (1 << 5))) { // checks for 0 to indicate valid data present
		vTaskDelay(1);
	}
	uint8_t data = LPC_UART3->RBR;

	return data;
}

bool UARTdriver::run(void *p)
{
	static uint8_t count=0;
	static uint8_t countingDown=0;
	char * name = "JAMES\n";

	if (uartMode == SEND_MODE || uartMode == BOTH_MODE) {
		printf("SENDING: 'JAMES'\n");
		UARTdriver::uart2_send(name[5-count]);
		count--;
		if (count == 0 && countingDown) {
			uartMode = NO_MODE;
			countingDown=0;
		} else if (!countingDown) {
			count=10;
			countingDown=1;
		}


	} else if (uartMode == LISTEN_MODE || uartMode == BOTH_MODE) {

		printf("%c\n",UARTdriver::uart3_receive());

	} else if (uartMode == BOTH_MODE) {
		if (!countingDown) {
			uartMode = NO_MODE;
		}
	}
	return true;
}

void UARTdriver::setMode(UARTMode mode){

	uartMode = mode;
	printf("changing mode!\n");
}


