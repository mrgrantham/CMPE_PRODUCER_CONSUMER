/*
 * eint.c
 *
 *  Created on: Mar 13, 2017
 *      Author: James
 */

void eint3_enable_port0(uint8_t pin_num, eint_intr_t type, void_func_t func) {

	NVIC_EnableIRQ(EINT3_IRQn);
}

void eint3_enable_port2(uint8_t pin_num, eint_intr_t type, void_func_t func) {

	NVIC_EnableIRQ(EINT3_IRQn);
}





