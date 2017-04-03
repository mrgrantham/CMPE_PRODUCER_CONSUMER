/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "test_switch.hpp"
#include "SPIdriver.hpp"
#include "UARTdriver.hpp"
#include "testEINT.hpp"
#include <stdio.h>
#include <test_task.hpp>
//#include "L4_IO/wireless/wireless.h"
#include "wireless.h"
#include "LED_display.hpp"
#include "L4_IO/wireless/src/mesh.h"
#include "L4_IO/wireless/src/mesh_typedefs.h"
#include "light_sensor.hpp"

void ping_and_set_led(uint8_t addr, uint8_t led_num)
{
    /* Sending NULL packet is a "PING" packet.
     * No special code is required at the other node since the
     * other node will automatically send the ACK back.
     */

	LED_Display inst = LED_Display::getInstance();

    const char max_hops = 2;
    mesh_packet_t pkt;
    mesh_send(addr, mesh_pkt_ack, NULL, 0, max_hops);

    /* Turn LED on or off based on if we get ACK packet within 100ms */
    if ( wireless_get_ack_pkt(&pkt, 100)) {
    	inst.setNumber(1);
        printf("pkt recvd\n");
    }
    else {
    	inst.setNumber(0);
        printf("NO pkt recvd\n");
    }
}

/* Common between commander and sensor node
 * This identifies "what" the commander is asking for
 */
enum {
    req_light = 1, /* Request light sensor reading       */
    req_all = 2,
};

void commander(void)
{
char hops = 1;
char addr = 125;
char cmd  = req_light; /* Request light sensor data */

/* mesh_pkt_ack_app means the destination should ACK manually
 * with our data, and an auto-ack is not performed.
 */
mesh_send(addr, mesh_pkt_ack_app, &cmd, 1, hops);

mesh_packet_t pkt;
if (wireless_get_ack_pkt(&pkt, 100)) {
     /* We need to deform packet the same way it was formed */
     /* Parameters should be same after 4th parameter into mesh_form_pkt() */
     uint16_t light = 0;
     wireless_deform_pkt(&pkt, 1, &light, sizeof(light));
 	LED_Display inst = LED_Display::getInstance();
 	inst.setNumber(light);

}
}

void sensor(void)
{
    mesh_packet_t pkt;
    uint16_t light = 0;

    if (wireless_get_rx_pkt(&pkt, 100)) {
        /* Check if we were asked for an application ACK */
        if (wireless_is_ack_required(&pkt)) {

            /* Send the packet back based on the commanded byte */
            const char cmd = pkt.data[0];
        	Light_Sensor LS = Light_Sensor::getInstance();

            switch (cmd) {
                case req_light :
                    /* Send packet back to network source: pkt.nwk.src */
                    light = LS.getRawValue();
                    wireless_form_pkt(&pkt, pkt.nwk.src, mesh_pkt_ack_rsp, 1,
                                      1,
                                      &light, sizeof(light));
                break;

                case req_all :
                    /* TODO: You figure this out.  You can send up to 24 bytes of data */
                break;

                default:
                    printf("ERROR: Invalid data requested!\n");
                break;
            }

            mesh_send_formed_pkt(&pkt);
        }
    }
}

class ping : public scheduler_task
{
    public:
        ping(uint8_t priority) : scheduler_task("task", 2000, priority)
        {
            /* Nothing to init */
        }

        bool run(void *p)
        {
            //ping_and_set_led(125, 1);
            commander();
        	//printf("sending from: %d\n", mesh_get_node_address());

            //vTaskDelay(1000);
            return true;
        }
};

/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at scheduler_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */


int main(void)
{
    /**
     * A few basic tasks for this bare-bone system :
     *      1.  Terminal task provides gateway to interact with the board through UART terminal.
     *      2.  Remote task allows you to use remote control to interact with the board.
     *      3.  Wireless task responsible to receive, retry, and handle mesh network.
     *
     * Disable remote task if you are not using it.  Also, it needs SYS_CFG_ENABLE_TLM
     * such that it can save remote control codes to non-volatile memory.  IR remote
     * control codes can be learned by typing the "learn" terminal command.
     */
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    /* Consumes very little CPU, but need highest priority to handle mesh network ACKs */
    scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));

    /* Change "#if 0" to "#if 1" to run period tasks; @see period_callbacks.cpp */
    #if 0
    scheduler_add_task(new periodicSchedulerTask());
    #endif

    /* The task for the IR receiver */
    // scheduler_add_task(new remoteTask  (PRIORITY_LOW));

    /* Your tasks should probably used PRIORITY_MEDIUM or PRIORITY_LOW because you want the terminal
     * task to always be responsive so you can poke around in case something goes wrong.
     */

    /**
     * This is a the board demonstration task that can be used to test the board.
     * This also shows you how to send a wireless packets to other boards.
     */
    #if 0
    	mesh_set_node_address(200);
        scheduler_add_task(new example_io_demo());
    #endif

	#if 0
        scheduler_add_task(new test_switch());
	#endif

	#if 0
        scheduler_add_task(new SPIdriver());
	#endif

	#if 0
		scheduler_add_task(new UARTdriver(PRIORITY_MEDIUM));
	#endif

	#if 0
		scheduler_add_task(new testEINT());
	#endif

	#if 1
		scheduler_add_task(new task1(PRIORITY_MEDIUM));
		scheduler_add_task(new task2(PRIORITY_MEDIUM));
//		xTaskCreate( task1, (const char*)"task1", 2048, 0 , PRIORITY_MEDIUM, 0 );
//		xTaskCreate( task2, (const char*)"task2", 2048, 0 , PRIORITY_MEDIUM, 0 );
	#endif

	#if 0
		LED_Display inst = LED_Display::getInstance();
		inst.setNumber(4);
		mesh_set_node_address(200);
		scheduler_add_task(new ping(PRIORITY_MEDIUM));
	#endif


    /**
     * Change "#if 0" to "#if 1" to enable examples.
     * Try these examples one at a time.
     */
    #if 0
        scheduler_add_task(new example_task());
        scheduler_add_task(new example_alarm());
        scheduler_add_task(new example_logger_qset());
        scheduler_add_task(new example_nv_vars());
    #endif

    /**
	 * Try the rx / tx tasks together to see how they queue data to each other.
	 */
    #if 0
        scheduler_add_task(new queue_tx());
        scheduler_add_task(new queue_rx());
    #endif

    /**
     * Another example of shared handles and producer/consumer using a queue.
     * In this example, producer will produce as fast as the consumer can consume.
     */
    #if 0
        scheduler_add_task(new producer());
        scheduler_add_task(new consumer());
    #endif

    /**
     * If you have RN-XV on your board, you can connect to Wifi using this task.
     * This does two things for us:
     *   1.  The task allows us to perform HTTP web requests (@see wifiTask)
     *   2.  Terminal task can accept commands from TCP/IP through Wifly module.
     *
     * To add terminal command channel, add this at terminal.cpp :: taskEntry() function:
     * @code
     *     // Assuming Wifly is on Uart3
     *     addCommandChannel(Uart3::getInstance(), false);
     * @endcode
     */
    #if 0
        Uart3 &u3 = Uart3::getInstance();
        u3.init(WIFI_BAUD_RATE, WIFI_RXQ_SIZE, WIFI_TXQ_SIZE);
        scheduler_add_task(new wifiTask(Uart3::getInstance(), PRIORITY_LOW));
    #endif

    scheduler_start(); ///< This shouldn't return
    return -1;
}
