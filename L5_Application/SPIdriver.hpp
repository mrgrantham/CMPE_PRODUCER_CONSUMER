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

enum SSPMode {
	OFF_MODE,
	ID_MODE,
	STATUS_MODE,
	DATA_MODE
};

class SPIdriver : public scheduler_task {
private:
	static SSPMode sspMode;
public:
    SPIdriver();
    bool run(void *p);
    bool init(void);
    void chip_select_enable();
	void chip_select_disable();
    static void printBinary(uint8_t num);
    static char byte_transfer(char aByte);
    static void get_device_id();
    static void get_status();
};



#endif /* L5_APPLICATION_SPIDRIVER_HPP_ */
