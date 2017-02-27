/*
 * SPIdriver.cpp
 *
 *  Created on: Feb 21, 2017
 *      Author: James
 */

#include "SPIdriver.hpp"
#include <stdio.h>
#include "LPC17xx.h"


SPIdriver::SPIdriver() :
	scheduler_task("SPI_driver", 4 * 512, PRIORITY_LOW)
{
    setRunDuration(500);


	//	1. Power: In the PCONP register (Table 46), set bit PCSSP0 to enable SSP0 and bit
	//	PCSSP1 to enable SSP1.
	//	Remark: On reset, both SSP interfaces are enabled (PCSSP0/1 = 1).

	LPC_SC->PCONP |= (1 << 10);


	// P0.6 Chip Select // active low
	// P0.7 SCK1

	// P0.8 MISO1
	// P0.9 MOSI1

	//	2. Clock: In PCLKSEL0 select PCLK_SSP1; in PCLKSEL1 select PCLK_SSP0 (see
	//	Section 4.7.3. In master mode, the clock must be scaled down (see Section 18.6.5).

	// set clock divider at higher level (if needed)
	LPC_SC->PCLKSEL0 &= ~(3 << 20);

	// set divider at SPI level
	LPC_SSP1->CPSR = 2;  //48mhz /2 24mhz SPI clock

	//	3. Pins: Select the SSP pins through the PINSEL registers (Section 8.5) and pin modes
	//	through the PINMODE registers (Section 8.4).

	// set (SCK1) P0.7
	LPC_PINCON->PINSEL0 &= ~(1 << 14); // set 15:14 to 2 (10)
	LPC_PINCON->PINMODE0 &= ~(3 << 14); // set 15:14 to 0 (00)

	// set (MISO1) P0.8

	LPC_PINCON->PINSEL0 &= ~(1 << 16); // set 17:16 to 2 (10)
	LPC_PINCON->PINMODE0 &= ~(3 << 16); // set 17:16 to 0 (00)


	// set (MOSI1) P0.9

	LPC_PINCON->PINSEL0 &= ~(1 << 18); // set 19:18 to 2 (10)
	LPC_PINCON->PINMODE0 &= ~(3 << 18); // set 19:18 to 0 (00)

	//	4. Interrupts: Interrupts are enabled in the SSP0IMSC register for SSP0 and SSP1IMSC
	//	register for SSP1 Table 376. Interrupts are enabled in the NVIC using the appropriate
	//	Interrupt Set Enable register, see Table 50.

	// ignore here

	//	5. Initialization: There are two control registers for each of the SSP ports to be
	//	configured: SSP0CR0 and SSP0CR1 for SSP0, SSP1CR0 and SSP1CR1 for SSP1.
	//	See Section 18.6.1 and Section 18.6.2.

	LPC_SSP1->CR0 |= ( 7 << 0 ); // 0b0111 on 3:0 bits sets data size to 8-bits
	LPC_SSP1->CR1 |=  ( 1 << 1 ); // enable SSP1

	//	6. DMA: The Rx and Tx FIFOs of the SSP interfaces can be connected to the GPDMA





}

bool SPIdriver::run(void *p){
	// choose action based on mode


	return true;
}

char SPIdriver::byte_transfer(char aByte) {
	// check page 37 on adesto chip
	LPC_SSP1->DR = aByte;

	return LPC_SSP1->DR;
}

void SPIdriver::get_device_id() {
	printf("running get_device_id()\n");
}
