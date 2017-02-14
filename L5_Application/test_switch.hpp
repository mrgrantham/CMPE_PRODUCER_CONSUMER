/*
 * test_switch.hpp
 *
 *  Created on: Feb 13, 2017
 *      Author: James
 */

#ifndef L5_APPLICATION_TEST_SWITCH_HPP_
#define L5_APPLICATION_TEST_SWITCH_HPP_

#include "scheduler_task.hpp"


class test_switch : public scheduler_task {
public:
    test_switch();
    bool run(void *p);
};



#endif /* L5_APPLICATION_TEST_SWITCH_HPP_ */
