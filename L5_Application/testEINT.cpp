/*
 * testEINT.cpp
 *
 *  Created on: Mar 12, 2017
 *      Author: James
 */

#include "testEINT.hpp"
#include <stdio.h>
#include "LPC17xx.h"
#include "lpc_sys.h"        // sys_reboot()
#include "printf_lib.h"

#include "eint.h"


    void callback1() {
    	//testEINT::setMode(CALL1_TEST);
		u0_dbg_printf("callback1\n");

    }

    void callback2() {
    	//testEINT::setMode(CALL2_TEST);
		u0_dbg_printf("callback2\n");

    }



	EINTMode eintMode;
	bool SW1 = false;
	bool SW2 = false;

	testEINT::testEINT() : scheduler_task("test_EINT", 4 * 512, PRIORITY_LOW) {
	    setRunDuration(100);
	}

	void configure_outputP0(int pin_num) {
    	uint8_t pin_num_adj = (pin_num << 1) % 32;

    	if ((pin_num * 2) < 32) {
    		LPC_PINCON->PINSEL0  &= ~( 3 << pin_num_adj );
    		LPC_PINCON->PINMODE0 &= ~( 3 << pin_num_adj );
    		LPC_PINCON->PINMODE0 |=  ( 1 << pin_num_adj );
    	} else {
    		LPC_PINCON->PINSEL1  &= ~( 3 << pin_num_adj );
    		LPC_PINCON->PINMODE1 &= ~( 3 << pin_num_adj );
    		LPC_PINCON->PINMODE1 |=  ( 1 << pin_num_adj );
    	}

    	LPC_GPIO0->FIODIR &= ~( 1 << pin_num );

	}

	void configure_outputP2(int pin_num) {
		LPC_PINCON->PINSEL2  &= ~( 3 << pin_num );
		LPC_PINCON->PINMODE2 &= ~( 3 << pin_num );
		LPC_PINCON->PINMODE2 |=  ( 1 << pin_num );
    	LPC_GPIO2->FIODIR &= ~( 1 << pin_num );

	}

    bool testEINT::init(void) {
    	configure_outputP0(0);
    	configure_outputP2(7);
		enable_external_interrupts();
    	return true;
    }

    bool testEINT::run(void *p) {
    	// task is activated on interrupt
    	// gets called with a slow enough delay for a second switch signal not to mess it up
    	if(eintMode == EINT_OFF) {
//			printf("EINT OFF\n");
        	testEINT::setMode(NIL_MODE);
		} else if(eintMode == EINT_ON) {
			printf("EINT ON\n");
        	testEINT::setMode(NIL_MODE);
		} else if(eintMode == CALL1_TEST) {
			//printf("CALL1\n");
			u0_dbg_printf("callback1\n");
        	testEINT::setMode(NIL_MODE);
		} else if(eintMode == CALL2_TEST) {
			//printf("CALL2\n");
			u0_dbg_printf("callback2\n");
        	testEINT::setMode(NIL_MODE);
		}

    	return true;
    }

    void testEINT::enable_external_interrupts() {
    	eint3_enable_port0(0, eint_rising_edge, &callback1);
    	eint3_enable_port2(7, eint_rising_edge, &callback2);
    }

    void testEINT::disable_external_interrupt() {
    	NVIC_DisableIRQ(EINT3_IRQn);
    }

    void testEINT::setMode(EINTMode mode) {

    }

