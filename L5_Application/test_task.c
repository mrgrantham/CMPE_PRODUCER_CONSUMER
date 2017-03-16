/*
 * test_task.c
 *
 *  Created on: Mar 15, 2017
 *      Author: James
 */

#include "uart0_min.h"
#include "test_task.h"

void task1(void * p) {
	 uart0_puts("aaaaaaaaaaaaaaaaaaaaaa");
}

void task2(void * p) {
	 uart0_puts("bbbbbbbbbbbbbbbb");
}
