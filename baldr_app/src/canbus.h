//********************************************************************************************************
// Filename		: H-file
// Description	: Handling Canbus
//********************************************************************************************************
#ifndef _CANBUS_H_INCLUDED	/* Allow multiple inclusions											    */
#define _CANBUS_H_INCLUDED

#include <zephyr/logging/log.h>
#include <zephyr/drivers/can.h>

#define CAN_NODE DT_CHOSEN(zephyr_canbus)
#define VESC_ID1							1 // VESC-kontroller 1
#define VESC_ID2							2 // VESC-kontroller 2
#define VESC_ID3       			        	3 // VESC-kontroller 3
#define VESC_ID4         					4 // VESC-kontroller 4
#define CAN_PACKET_SET_DUTY     			0 // Dette er PWM-kommandoer
#define CAN_PACKET_SET_RPM      			3 // Dette er ERPM-kommandoer

extern const struct device *can_dev;

//********************************************************************************************************
int vesc_set_rpm(int32_t rpm, int32_t vesc_id);

#endif /* _CANBUS_H_INCLUDED																			*/
