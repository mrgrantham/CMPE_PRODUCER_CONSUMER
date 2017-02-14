/*
 * test_switch.cpp
 *
 *  Created on: Feb 13, 2017
 *      Author: James
 */

#include "test_switch.hpp"
#include <stdio.h>
#include "LPC17xx.h"

test_switch::test_switch() :
        scheduler_task("switch_tester", 4 * 512, PRIORITY_LOW)
{
    setRunDuration(500);

    // set P1.28 to GPIO
    LPC_PINCON->PINSEL3 &= ~(3<<24);
    // set P1.23 to GPIO
    LPC_PINCON->PINSEL3 &= ~(3<<14);

    // set P1.28 to nothing Mode
    LPC_PINCON->PINMODE3 &= ~(2<<24);
    // set P1.23 to nothing mode
    LPC_PINCON->PINMODE3 &= ~(2<<14);

	// set direction of P1.28 to input
	LPC_GPIO1->FIODIR &= ~(1<<28);
	// set direction of p1.23 to output
	LPC_GPIO1->FIODIR |= (1<<23);

}

bool test_switch::run(void *p)
{

	if(LPC_GPIO1->FIOPIN & (1 << 28)) {
		printf("switch HIGH\n");
		LPC_GPIO1->FIOPIN |= (1 << 23);
	} else {
		printf("switch LOW\n");
		LPC_GPIO1->FIOPIN &= ~(1 << 23);
	}

//	//toggle
//	static int8_t s_status = 0;
//
//	if (s_status == 0) {
//		LPC_GPIO1->FIOPIN |= (1 << 23);
//		s_status = 1;
//	} else {
//		LPC_GPIO1->FIOPIN &= ~(1 << 23);
//		s_status = 0;
//	}

//    static uint8_t pkt_counter = 0;
//    const uint8_t switches = SW.getSwitchValues();
//    mesh_packet_t pkt;
//
//    enum {
//        sw1 = (1 << 0),
//        sw2 = (1 << 1),
//        sw3 = (1 << 2),
//        sw4 = (1 << 3),
//    };
//
//    switch(switches)
//    {
//        case sw1 :
//            printf("Acceleration: %4i %4i %4i\n", AS.getX(), AS.getY(), AS.getZ());
//            LD.setNumber(AS.getX());
//            break;
//
//        case sw2 :
//            printf("Light : %4d\n", LS.getRawValue());
//            LD.setNumber(LS.getRawValue());
//            break;
//
//        case sw3 :
//            printf("Temperature: %i\n", (int) TS.getFarenheit());
//            LD.setNumber(TS.getFarenheit());
//            break;
//
//        case sw4 :
//            /* Send broadcast message, and increment led number if we get a packet back */
//            if (!wireless_send(MESH_BROADCAST_ADDR, mesh_pkt_nack, (char*)&pkt_counter, 1, 0)) {
//                puts("Failed to send packet");
//            }
//            else if (wireless_get_rx_pkt(&pkt, 500)) {
//                LD.setNumber(pkt_counter++);
//            }
//            else {
//                puts("Broadcast message not received!");
//            }
//            break;
//
//        default :
//            break;
//    }
//
//    /* Only set LEDs when one of the switches are pressed, or LEDs are ON
//     * If LEDs are on, this will turn them off.
//     */
//    if(switches || 0 != LE.getValues()) {
//        LE.setAll(switches);
//    }
//
//    /* If we receive a broadcast message with 1 byte, send something back */
//    if (wireless_get_rx_pkt(&pkt, 0) && 1 == pkt.info.data_len) {
//        wireless_send(MESH_BROADCAST_ADDR, mesh_pkt_nack, "ack", 3, 0);
//        pkt_counter = pkt.data[0];
//        LD.setNumber(pkt_counter);
//    }

    return true;
}


