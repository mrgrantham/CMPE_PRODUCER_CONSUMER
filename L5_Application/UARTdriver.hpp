/*
 * UARTdriver.hpp
 *
 *  Created on: Mar 5, 2017
 *      Author: James
 */

#ifndef UARTDRIVER_HPP_
#define UARTDRIVER_HPP_

// UART Driver Code
#include "stdint.h"
#include "scheduler_task.hpp"


// Must use different UART from partner
// Must have common ground
//

enum UARTMode {
	NO_MODE,
	SEND_MODE,
	LISTEN_MODE,
	BOTH_MODE
};

class UARTdriver : public scheduler_task {
private:
	static UARTMode uartMode;

public:
        UARTdriver(uint8_t priority);
        bool init(void);
        bool run(void *p);
        static void uart2_send(char data);
        static char uart2_receive();
        static void uart3_send(char data);
        static char uart3_receive();
        static void setMode(UARTMode mode);
};





#endif /* UARTDRIVER_HPP_ */
