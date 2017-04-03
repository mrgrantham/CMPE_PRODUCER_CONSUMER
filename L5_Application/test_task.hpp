/*
 * test_task.h
 *
 *  Created on: Mar 15, 2017
 *      Author: James
 */

#ifndef TEST_TASK_H_
#define TEST_TASK_H_

#include "scheduler_task.hpp"


typedef enum {
	shared_AccelQueueID
} sharedHandleID_t;

typedef enum Orientation {
	none,
	down,
	up,
	left,
	right,
	forward,
	backward

} Orientation;


class task1 : public scheduler_task {
public:
	task1(uint8_t priority);
	bool run(void *p);
};

class task2 : public scheduler_task {
public:
	task2(uint8_t priority);
	bool run(void *p);
};



#endif /* TEST_TASK_H_ */
