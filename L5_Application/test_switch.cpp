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
LedMode test_switch::ledMode;
uint8_t test_switch::sos_seq[MAX_FRAMES] = {0,0,1,1,0,1,1,0,1,1,0,0,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,0,0,0,1,1,0,1,1,0,1,1,0,0,0,0,0,0,0,0};
uint8_t test_switch::frame=0;
str test_switch::pattern;
uint8_t test_switch::plen;
uint32_t test_switch::switch_state_prev=0;

test_switch::test_switch() :
        scheduler_task("switch_tester", 4 * 512, PRIORITY_LOW)
{
    setRunDuration(100);

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

//	printf("run switch\n");
	if(ledMode == LED_OFF) {
		LPC_GPIO1->FIOPIN &= ~(1 << 23);
		ledStatus = false;
	} else if(ledMode == LED_ON) {
		LPC_GPIO1->FIOPIN |= (1 << 23);
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
	} else if(ledMode == LED_SOS) {
		if(sos_seq[frame]) {
			//printf("strobe HIGH\n");
			LPC_GPIO1->FIOPIN |= (1 << 23);
			ledStatus = true;
		} else {
			//printf("strobe LOW\n");
			LPC_GPIO1->FIOPIN &= ~(1 << 23);
			ledStatus = false;
		}

		frame ++;
		frame %= MAX_FRAMES;

	} else if (ledMode == LED_PATTERN) {
		if(pattern[frame] == '1') {
			LPC_GPIO1->FIOPIN |= (1 << 23);
			ledStatus = true;
		} else {
			LPC_GPIO1->FIOPIN &= ~(1 << 23);
			ledStatus = false;
		}

		frame ++;
		frame %= plen;
	}

	if ((LPC_GPIO1->FIOPIN & (1 << 28)) != switch_state_prev) {
		setMode(LED_SWITCH);
		switch_state_prev = LPC_GPIO1->FIOPIN & (1 << 28);
	}


    return true;
}

void test_switch::setMode(LedMode mode) {
	test_switch::ledMode = mode;
	test_switch::frame = 0;
}

void test_switch::setPattern(str pat) {
	pattern = pat;
	plen = pattern.getLen();
}

