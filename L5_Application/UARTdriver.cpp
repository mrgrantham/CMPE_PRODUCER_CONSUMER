/*
 * UARTdriver.cpp
 *
 *  Created on: Mar 5, 2017
 *      Author: James
 */

#include "UARTdriver.hpp"
#include "LPC17xx.h"
#include <stdio.h>
#include "printf_lib.h"
#include "string.h"

UARTMode UARTdriver::uartMode = NO_MODE;
uint8_t UARTdriver::max_count=100;


UARTdriver::UARTdriver(uint8_t priority) : scheduler_task("task", 2000, priority)
{
	/* Nothing to init */
	//setRunDuration();
}

bool UARTdriver::init(void)
{
	LPC_SC->PCONP |= (1 << 24); // turn on the UART
	LPC_SC->PCONP |= (1 << 25); // turn on the UART

	// set clock divider

	// for uart2
	LPC_SC->PCLKSEL1 &= ~(3 << 16); // clear the 2 bits
	LPC_SC->PCLKSEL1 |= (1 << 16); // set the 1 for clock divider

	// for uart3
	LPC_SC->PCLKSEL1 &= ~(3 << 18); // clear the 2 bits
	LPC_SC->PCLKSEL1 |= (1 << 18); // set the 1 for clock divider

	// 21:20 P0.10 TXD2
	// 23:22 P0.11 RXD2

	// set baud rate divider

	LPC_UART2->LCR |= ( 1 << 7 );
	LPC_UART3->LCR |= ( 1 << 7 );


	const uint32_t  baud_rate = 9600;
	const uint16_t div16 = sys_get_cpu_clock() / (16 * baud_rate);

	LPC_UART2->DLL = (div16 & 0xff); // lower byte
	LPC_UART2->DLM = (div16 >> 8); // upper byte

	LPC_UART3->DLL = (div16 & 0xff) ; // lower byte
	LPC_UART3->DLM = (div16 >> 8); // upper byte


	LPC_PINCON->PINSEL4 &= ~(3 << 16);
	LPC_PINCON->PINSEL4 |= (2 << 16);   // TXD2

	LPC_PINCON->PINSEL4 &= ~(3 << 18);
	LPC_PINCON->PINSEL4 |= (2 << 18);   // RXD2

	LPC_PINCON->PINSEL9 |= (3 << 24);   // TXD3
	LPC_PINCON->PINSEL9 |= (3 << 26);   // RXD3

	// set to 8-bit length
	LPC_UART2->LCR = 3;
	LPC_UART3->LCR = 3;

	// set 2 stop bits
	LPC_UART2->LCR |= (1 << 2);
	LPC_UART3->LCR |= (1 << 2);

	// enable RBR interrupt
//	LPC_UART2->IER |= (1 << 0);
//	LPC_UART3->IER |= (1 << 0);

	return true;
}

void UARTdriver::uart2_send(char data) {
	uart_send(data, LPC_UART2);
}

char UARTdriver::uart2_receive() {
	return uart_receive(LPC_UART3);
}

void UARTdriver::uart3_send(char data) {
	uart_send(data, LPC_UART3);
}

char UARTdriver::uart3_receive() {
	return uart_receive(LPC_UART3);
}

void UARTdriver::uart_send(char data, LPC_UART_TypeDef *uartStruct) {
	while (!(uartStruct->LSR & (1 << 6))) {
		// add sleep function
		//vTaskDelay(1);
	}
	uartStruct->THR = data; //sending byte
}

char UARTdriver::uart_receive(LPC_UART_TypeDef *uartStruct) {

	//check status register and then read
	uint8_t data = 0;
	if (uartStruct->LSR & (1 << 0)) { // checks for 0 to indicate valid data present
		data = uartStruct->RBR;
	} else {
		data = 0;
	}
	return data;

}


bool UARTdriver::run(void *p) {
	static uint8_t count=0;
	static uint8_t counting=0;
	static char * message = "JAMES ANDREW GRANTHAM 010766912 SJSU MASTERS OF COMPUTER ENGINEERING STUDENT CMPE244CMPE244CMPE244\n";
	static uint16_t messagelen=0;
	static uint16_t messageIndex;

	static uint8_t first_run = 1;
	if (first_run) {
		messagelen = strlen(message);
		first_run = 0;
	}

	if (count == max_count && counting) {

		uartMode = NO_MODE;
		u0_dbg_printf("ENDING %s MODE\n", (SEND_MODE)? "SEND" : ((LISTEN_MODE)? "LISTEN" : "ROUNDTRIP" ) );
		counting=0;
	} else if (!counting && (uartMode == BOTH_MODE || uartMode == LISTEN_MODE || uartMode == SEND_MODE)) {
		count=0;
		counting=1;
	}

	if (uartMode == SEND_MODE || uartMode == BOTH_MODE) {
		//u0_dbg_printf("|");
		UARTdriver::uart2_send(message[messageIndex]);
		messageIndex++;
		messageIndex %= messagelen;
	}
	if (uartMode == LISTEN_MODE || uartMode == BOTH_MODE) {
		uint8_t cVal = UARTdriver::uart2_receive();
//		u0_dbg_printf("%c",UARTdriver::uart3_receive());
		if (cVal) {
			u0_dbg_printf("%c",cVal);
		}
	}
	if (count < max_count && counting) {
		count++;
	}

	return true;
}

void UARTdriver::setMode(UARTMode mode){

	uartMode = mode;
	u0_dbg_printf("changing mode!\n");
}

void UARTdriver::setMaxCount(uint8_t max) {
	max_count = max;
}


