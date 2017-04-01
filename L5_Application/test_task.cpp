/*
 * test_task.cpp
 *
 *  Created on: Mar 15, 2017
 *      Author: James
 */

#include <test_task.hpp>
#include "uart0_min.h"
#include "scheduler_task.hpp"
#include <stdio.h>
#include "io.hpp"

QueueHandle_t qh = NULL;

void task1(void * p) {
	qh =  xQueueCreate( 100, sizeof(Orientation) );
	Orientation ot = none;
	while(1) {
		 //uart0_puts("aaaaaaaaaaaaaaaaaaaaaa");
		static int16_t x;
		static int16_t y;
		static int16_t z;
		x = AS.getX();
		y = AS.getY();
		z = AS.getZ();
		 printf("AC x:%3d y:%3d z:%3d\n",x,y,z);
		 vTaskDelay(300);
		 if (z > 1000 && x < 100 && y < 100) {
			 ot = up;
			 xQueueSend(qh, &ot, portMAX_DELAY);
		 }

	}
}

void task2(void * p) {
	Orientation ot = none;
	while(1) {
		if (qh) {
			xQueueReceive(qh, &ot, portMAX_DELAY);
			switch (ot) {
			case up:
				printf("TOP\n");
				break;
			case down:
				printf("BOTTOM\n");
				break;
			case left:
				printf("LEFT\n");
				break;
			case right:
				printf("RIGHT\n");
				break;
			default:
				printf("ERROR\n");
				break;
			}
		}
		vTaskDelay(300);
	}
}
