/*
 * test_switch.hpp
 *
 *  Created on: Feb 13, 2017
 *      Author: James
 */

#ifndef L5_APPLICATION_TEST_SWITCH_HPP_
#define L5_APPLICATION_TEST_SWITCH_HPP_

#include "scheduler_task.hpp"
#include "stdint.h"
#include "str.hpp"

#define MAX_FRAMES 46


enum LedMode {
	LED_OFF,
	LED_ON,
	LED_STROBE,
	LED_SWITCH,
	LED_SOS,
	LED_PATTERN
};


class test_switch : public scheduler_task {
private:
	static LedMode ledMode;
	static uint8_t sos_seq[MAX_FRAMES];
	static uint8_t frame;
	static str pattern;
	static uint8_t plen;
	static uint32_t switch_state_prev;
public:
	static bool ledStatus;
    test_switch();
    bool run(void *p);
    static void setMode(LedMode mode);
    static void setPattern(str pat);
};



#endif /* L5_APPLICATION_TEST_SWITCH_HPP_ */
