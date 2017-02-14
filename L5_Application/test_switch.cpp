/*
 * test_switch.cpp
 *
 *  Created on: Feb 13, 2017
 *      Author: James
 */

#include "test_switch.hpp"
#include <stdio.h>
#include "LPC17xx.h"

bool test_switch::ledStatus;
uint8_t test_switch::ledMode;

test_switch::test_switch() :
        scheduler_task("switch_tester", 4 * 512, PRIORITY_LOW)
{
    setRunDuration(500);

    test_switch::ledStatus = false;
    test_switch::ledMode = LED_OFF;
    setMode(LED_SWITCH);

    // set P1.28 to GPIO
    LPC_PINCON->PINSEL3 &= ~(3<<24);
    // set P1.23 to GPIO
    LPC_PINCON->PINSEL3 &= ~(3<<14);

    // set P1.28 to nothing Mode
    LPC_PINCON->PINMODE3 &= ~(2<<24);
    // set P1.23 to nothing mode
    LPC_PINCON->PINMODE3 &= ~(2<<14);

	// set direction of P1.28 to input
	LPC_GPIO1->FIODIR &= ~(1<<28);
	// set direction of p1.23 to output
	LPC_GPIO1->FIODIR |= (1<<23);

}

bool test_switch::run(void *p)
{

	if(ledMode == LED_OFF) {
		LPC_GPIO1->FIOPIN |= (1 << 23);
		ledStatus = false;
	} else if(ledMode == LED_ON) {
		LPC_GPIO1->FIOPIN &= ~(1 << 23);
		ledStatus = true;
	} else if(ledMode == LED_STROBE) {
		if(!ledStatus) {
			//printf("strobe HIGH\n");
			LPC_GPIO1->FIOPIN |= (1 << 23);
			ledStatus = true;
		} else {
			//printf("strobe LOW\n");
			LPC_GPIO1->FIOPIN &= ~(1 << 23);
			ledStatus = false;
		}
	} else if(ledMode == LED_SWITCH) {
		if(LPC_GPIO1->FIOPIN & (1 << 28)) {
			//printf("switch HIGH\n");
			LPC_GPIO1->FIOPIN |= (1 << 23);
			ledStatus = true;
		} else {
			//printf("switch LOW\n");
			LPC_GPIO1->FIOPIN &= ~(1 << 23);
			ledStatus = false;
		}
	}



    return true;
}

void test_switch::setMode(uint8_t mode) {

	test_switch::ledMode = mode;

}

