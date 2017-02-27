/*
 * SPIdriver.hpp
 *
 *  Created on: Feb 21, 2017
 *      Author: James
 */

#ifndef L5_APPLICATION_SPIDRIVER_HPP_
#define L5_APPLICATION_SPIDRIVER_HPP_

#include "scheduler_task.hpp"
#include "stdint.h"
#include "str.hpp"

class SPIdriver : public scheduler_task {

public:
    SPIdriver();
    bool run(void *p);
    char byte_transfer(char aByte);
    static void get_device_id();
};



#endif /* L5_APPLICATION_SPIDRIVER_HPP_ */
