//********************************************************************************************************
// Filename		: canbus.c
// Description	: Program taking Joystick input as speed reference and limits the accelaration
//********************************************************************************************************
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
//#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include "canbus.h"

LOG_MODULE_REGISTER(canbus);

const struct device *can_dev = DEVICE_DT_GET(CAN_NODE);

//********************************************************************************************************
int vesc_set_rpm(int32_t rpm, int32_t vesc_id)
{
    struct can_frame frame = {0};

    /* Bygg CAN-ID: (cmd << 8) | vesc_id */
    //uint32_t cmd_id = ((uint32_t)CAN_PACKET_SET_RPM << 8) | VESC_ID;
    uint32_t cmd_id = ((uint32_t)CAN_PACKET_SET_RPM << 8) | vesc_id;

    frame.id      = cmd_id;
    frame.dlc     = 4;   /* RPM = 4 byte */
	frame.flags   = CAN_FRAME_IDE;

    /* Pakk RPM som big-endian */
    frame.data[0] = (rpm >> 24) & 0xFF;
    frame.data[1] = (rpm >> 16) & 0xFF;
    frame.data[2] = (rpm >> 8)  & 0xFF;
    frame.data[3] = (rpm >> 0)  & 0xFF;

	int ret = can_send(can_dev, &frame, K_NO_WAIT, NULL, NULL);
    return ret;
}
