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

#define	LED_OFF 	0
#define	LED_ON 		1
#define	LED_STROBE 	2
#define	LED_SWITCH 	3

class test_switch : public scheduler_task {
public:
	static uint8_t ledMode;
	static bool ledStatus;
    test_switch();
    bool run(void *p);
    static void setMode(uint8_t mode);
};



#endif /* L5_APPLICATION_TEST_SWITCH_HPP_ */
