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


task1::task1(uint8_t priority)  : scheduler_task("task1", 2048, priority)
{

}

bool task1::run(void * p) {
	qh =  xQueueCreate( 100, sizeof(Orientation) );
	Orientation ot = none;
	 //uart0_puts("aaaaaaaaaaaaaaaaaaaaaa");
	static int16_t x;
	static int16_t y;
	static int16_t z;
	x = AS.getX();
	y = AS.getY();
	z = AS.getZ();
	 printf("AC x:%3d y:%3d z:%3d\n",x,y,z);
	 vTaskDelay(300);
	 if (z > 1000) {
		 ot = up;
	 } else if (x > 700) {
		 ot = left;
	 } else if (x < -700) {
		 ot = right;
	 } else if (z < -1000) {
		 ot = down;
	 } else if (y < -700) {
		 ot = forward;
	 } else if (y > 700) {
		 ot = backward;
	 } else {
		 printf("error detecting orientation\n");
	 }
	 xQueueSend(qh, &ot, portMAX_DELAY);
	 return true;
}

task2::task2(uint8_t priority): scheduler_task("task2", 2048, priority)
{

}

bool task2::run(void * p) {
	Orientation ot = none;
	if (qh) {
		xQueueReceive(qh, &ot, portMAX_DELAY);
		switch (ot) {
		case up:
			printf("UP\n");
			LE.off(1);
			LE.off(2);
			LE.off(3);
			LE.off(4);
			break;
		case down:
			printf("DOWN\n");
			LE.off(1);
			LE.off(2);
			LE.off(3);
			LE.off(4);
			break;
		case left:
			printf("LEFT\n");
			LE.on(1);
			LE.on(2);
			LE.on(3);
			LE.on(4);
			break;
		case right:
			printf("RIGHT\n");
			LE.on(1);
			LE.on(2);
			LE.on(3);
			LE.on(4);
			break;
		case forward:
			printf("FORWARD\n");
			LE.off(1);
			LE.off(2);
			LE.off(3);
			LE.off(4);
			break;
		case backward:
			printf("BACKWARD\n");
			LE.off(1);
			LE.off(2);
			LE.off(3);
			LE.off(4);
			break;
		default:
			printf("ERROR\n");
			LE.off(1);
			LE.off(2);
			LE.off(3);
			LE.off(4);
			break;
		}
		vTaskDelay(300);
	}
	return true;
}
