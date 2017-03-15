/*
 * testEINT.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: James
 */

#ifndef TESTEINT_HPP_
#define TESTEINT_HPP_

#include "scheduler_task.hpp"
#include "stdint.h"
#include "str.hpp"

enum EINTMode {
	EINT_OFF,
	EINT_ON,
	EINT_TEST,
	NIL_MODE,
	GEN_EINT_TEST
};

class testEINT : public scheduler_task {
private:
	static EINTMode currMode;
	static EINTMode prevMode;

	static bool SW1;
	static uint32_t SW1port;
	static uint32_t SW1pin;

	static bool SW2;
	static uint32_t SW2port;
	static uint32_t SW2pin;


public:
    testEINT();
    bool init(void);
    bool run(void *p);
    static void enable_external_interrupts();
    static void disable_external_interrupt();
    static void setMode(EINTMode mode);
};





#endif /* TESTEINT_HPP_ */
