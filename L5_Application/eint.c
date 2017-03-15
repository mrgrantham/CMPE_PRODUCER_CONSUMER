/*
 * eint.c
 *
 *  Created on: Mar 13, 2017
 *      Author: James
 */
#include "eint.h"
#include "lpc_sys.h"
#include "LPC17xx.h"
#include "printf_lib.h"

typedef struct callback_s {
	uint32_t pin;
	void_func_t func;
	bool rise;
}  callback_s_t;

static callback_s_t port0callbacks[32];
static uint16_t port0callbacksCount = 0;
static callback_s_t port2callbacks[32];
static uint16_t port2callbacksCount = 0;

void EINT3_IRQHandler(void) {

			static int count = 0;
			count++;
			//printf("SET\n");
			//u0_dbg_printf("SET\n");

			for (int i = 0; i < port0callbacksCount; i++) {
				if (port0callbacks[i].rise) {
					if (LPC_GPIOINT->IO0IntStatR & (1 << port0callbacks[i].pin)) {
						(*port0callbacks[i].func)();
						LPC_GPIOINT->IO0IntClr = (1<<port0callbacks[i].pin);
					}
				} else {
					if (LPC_GPIOINT->IO0IntStatF & (1 << port0callbacks[i].pin)) {
						(*port0callbacks[i].func)();
						LPC_GPIOINT->IO0IntClr = (1<<port0callbacks[i].pin);
					}
 				}
			}

			for (int i = 0; i < port2callbacksCount; i++) {
				if (port2callbacks[i].rise) {
					if (LPC_GPIOINT->IO2IntStatR & (1 << port2callbacks[i].pin)) {
						(*port2callbacks[i].func)();
						LPC_GPIOINT->IO2IntClr = (1<<port2callbacks[i].pin);
					}
				} else {
					if (LPC_GPIOINT->IO2IntStatF & (1 << port2callbacks[i].pin)) {
						(*port2callbacks[i].func)();
						LPC_GPIOINT->IO2IntClr = (1<<port2callbacks[i].pin);
					}
 				}
			}
            LPC_GPIOINT->IO2IntClr = 0xFFFFFFFF;
            LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;

}

void eint3_enable_port0(uint8_t pin_num, eint_intr_t type, void_func_t func) {


	if (type == eint_rising_edge) {
		LPC_GPIOINT->IO0IntEnR |= ( 1 << pin_num );
	} else if (type == eint_falling_edge) {
		LPC_GPIOINT->IO0IntEnF |= ( 1 << pin_num );
	}

	port0callbacks[port0callbacksCount].pin = pin_num;
	port0callbacks[port0callbacksCount].func = func;
	port0callbacks[port0callbacksCount].rise = type;
	port0callbacksCount++;


	NVIC_EnableIRQ(EINT3_IRQn);
}

void eint3_enable_port2(uint8_t pin_num, eint_intr_t type, void_func_t func) {
	// only 0-13 available
	if (type == eint_rising_edge) {
		LPC_GPIOINT->IO2IntEnR |= ( 1 << pin_num );
	} else if (type == eint_falling_edge) {
		LPC_GPIOINT->IO2IntEnF |= ( 1 << pin_num );
	}

	port2callbacks[port2callbacksCount].pin = pin_num;
	port2callbacks[port2callbacksCount].func = func;
	port2callbacks[port2callbacksCount].rise = type;
	port2callbacksCount++;


	NVIC_EnableIRQ(EINT3_IRQn);
}





