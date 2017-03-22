/*
 * test_task.cpp
 *
 *  Created on: Mar 15, 2017
 *      Author: James
 */

#include <test_task.hpp>
#include "uart0_min.h"
#include "scheduler_task.hpp"

void task1(void * p) {
	while(1) {
//		 vTaskDelay(1000);
		 uart0_puts("aaaaaaaaaaaaaaaaaaaaaa");
	}
}

void task2(void * p) {
	while(1) {
//		 vTaskDelay(1000);
		 uart0_puts("bbbbbbbbbbbbbbbb");
	}
}
