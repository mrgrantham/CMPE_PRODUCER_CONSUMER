/*
 * SPIdriver.cpp
 *
 *  Created on: Feb 21, 2017
 *      Author: James
 */

#include "SPIdriver.hpp"
#include <stdio.h>
#include "LPC17xx.h"
#include "printf_lib.h"

SSPMode SPIdriver::sspMode = OFF_MODE;


SPIdriver::SPIdriver() :
	scheduler_task("SPI_driver", 4 * 512, PRIORITY_LOW)
{
	printf("per-run duration\n");
	setRunDuration(200);
}

bool SPIdriver::run(void *p){
	// choose action based on mode

	//printf("run\n");
	//u0_dbg_printf("run\n");

	if (sspMode == ID_MODE) {
//		printf("Running ID_MODE\n");
		uint8_t mfID;
		uint8_t devID[2];
		uint8_t EDIstrLen;
		uint8_t EDIstr[1];
		int16_t count=0;

		chip_select_enable();
		byte_transfer((uint8_t)0x9F);
		mfID = 		byte_transfer((uint8_t)0x9F);
		devID[1] = 	byte_transfer((uint8_t)0x9F);
		devID[0] = 	byte_transfer((uint8_t)0x9F);
		EDIstrLen = 	byte_transfer((uint8_t)0x9F);
		EDIstr[0] = 0;
		for (count = 0; count < EDIstrLen;count++) {
			EDIstr[count] = 	byte_transfer((uint8_t)0x9F);
		}

		printf("\n");
		printf("Manufacter ID: %02x\n",mfID);
		printf("Device ID: %04x\n",*((uint16_t*)devID));
		printf("Extended Device Information String Length: %d\n", EDIstrLen);
		printf("Extended Device Information String: %02x\n",(uint8_t)EDIstr[0]);

		chip_select_disable();
		printf("\n");
		sspMode = OFF_MODE;

	} else if (sspMode == STATUS_MODE) {

		uint8_t byte1;
		uint8_t byte2;

		uint8_t rdybsy1;
		uint8_t compare;
		uint8_t density=0;
		uint8_t protect;
		uint8_t pagesize;

		uint8_t rdybsy2;
		// bit 6 is reserved
		uint8_t eraseOrProgErr;
		// bit 4 is reserved
		uint8_t sectorLockdown;
		uint8_t progSuspendStatus1;
		uint8_t progSuspendStatus2;
		uint8_t eraseSuspend;


		chip_select_enable();

		byte_transfer((uint8_t)0xD7);
		byte1 = byte_transfer((uint8_t)0xD7);
		byte2 = byte_transfer((uint8_t)0xD7);

		chip_select_disable();

		rdybsy1 = byte1 & ( 1 << 7 );
		compare = byte1 & ( 1 << 6 );
		density = (byte1 & ( 0xf << 2 )) >> 2;
		protect = byte1 & ( 1 << 1 );
		pagesize = byte1 & ( 1 << 0 );

		rdybsy2 				= byte2 & ( 1 << 7 );
		// bit 6 is reserved
		eraseOrProgErr 			= byte2 & ( 1 << 5 );
		// bit 4 is reserved
		sectorLockdown 			= byte2 & ( 1 << 3 );
		progSuspendStatus1 		= byte2 & ( 1 << 2 );
		progSuspendStatus2 		= byte2 & ( 1 << 1 );
		eraseSuspend 			= byte2 & ( 1 << 0 );

		printf("\n--------------------");
		printf("\nBYTE1: 0b");
		printBinary(byte1);
		printf("\n--------------------");
		printf("\n");
		printf("RDY/BUSY: %s\n", rdybsy1? "READY" : "BUSY");
		printf("COMPARE: %s\n", compare? "DOES NOT MATCH BUFFER DATA": "MATCHES BUFFER DATA");
		printf("DENSITY: %d\n",density);
		printf("SECTOR PROTECTION STATUS: %s\n", protect? "ENABLED" : "DISABLED");
		printf("PAGE SIZE: %s\n",pagesize? "POWER OF 2 SIZE (512b)" : "DATAFLASH SIZE (528b)");

		printf("\n--------------------");
		printf("\nBYTE2: 0b");
		printBinary(byte2);
		printf("\n--------------------");
		printf("\n");

		printf("RDY/BUSY: %s\n", rdybsy2? "READY" : "BUSY");
		printf("ERASE/PROGRAM ERROR: %s\n", eraseOrProgErr? "ERASE OR PROG ERROR" : "ERASE OR PROG SUCCESSFUL");
		printf("SECTOR LOCKDOWN STATUS: %s\n", sectorLockdown? "ENABLED" : "DISABLED");
		printf("PROG SUSPEND (BUFFER 2): %s\n", progSuspendStatus2? "SECTOR PROG SUSPENDED WHILE USING BUFFER 2" : "NO PROG OPERATION SUSPENDED WHILE USING BUFFER 2");
		printf("PROG SUSPEND (BUFFER 1): %s\n", progSuspendStatus1? "SECTOR PROG SUSPENDED WHILE USING BUFFER 1" : "NO PROG OPERATION SUSPENDED WHILE USING BUFFER 1");
		printf("ERASE SUSPEND: %s\n", eraseSuspend? "A SECTOR IS ERASE SUSPENDED" :"NO SECTORS ARE ERASE SUSPENDED");

		sspMode = OFF_MODE;
	}

	return true;
}

bool SPIdriver::init(void) {


	printf("SSP1 initializing\n");
    // set the Chip Select Control

    // set P0.06 to GPIO
    LPC_PINCON->PINSEL0 &= ~(3<<12);

    // set P0.06 to nothing mode
    LPC_PINCON->PINMODE0 &= ~(2<<12);

	// set direction of p0.06 to output
	LPC_GPIO0->FIODIR |= (1<<6);

	//	1. Power: In the PCONP register (Table 46), set bit PCSSP0 to enable SSP0 and bit
	//	PCSSP1 to enable SSP1.
	//	Remark: On reset, both SSP interfaces are enabled (PCSSP0/1 = 1).

	LPC_SC->PCONP |= (1 << 10);

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


	printf("SSP1 initialized\n");

	return true;
}

void SPIdriver::chip_select_enable(){
	// acitive low
	LPC_GPIO0 -> FIOCLR = (1 << 6);
}

void SPIdriver::chip_select_disable(){
	LPC_GPIO0 -> FIOSET = (1 << 6);
}

char SPIdriver::byte_transfer(char aByte) {

	// check page 37 on adesto chip
	LPC_SSP1->DR = aByte;
	while (LPC_SSP1->SR & (1<<4));
	int retByte = LPC_SSP1->DR;

	return retByte;
}


void SPIdriver::printBinary(uint8_t num) {
    //printf("0b");
    uint8_t temp = 0;
    for (int8_t i=7;i>=0;i--) {
        temp=num;
        temp>>=i;
        temp &= 0x01;
        printf("%d", temp);
    }
}


void SPIdriver::get_device_id() {
//	printf("running get_device_id()\n");
//	for (int i = 0; i < 20; i+=3) {
//		printf("testing binary: %d: ",i);
//		printBinary(i);
//		printf("\n");
//	}
	sspMode = ID_MODE;

}

void SPIdriver::get_status() {
//	printf("running get_status()\n");

	sspMode = STATUS_MODE;

}

