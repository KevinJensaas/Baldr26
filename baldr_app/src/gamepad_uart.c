#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>

#include "gamepad_uart.h"
#include "joystick.h"

LOG_MODULE_REGISTER(gamepad_uart);

#define UART_NODE DT_NODELABEL(uart1)

static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

gamepad_t gamepad = {0};

//****************************************************************************************************
void gamepad_uart_init(void)
{
    if (!device_is_ready(uart_dev)) {
        LOG_ERR("UART not ready");
        return;
    }

    LOG_INF("Gamepad UART ready");
}

//****************************************************************************************************

void gamepad_uart_update(void)
{
    //LOG_INF("update called");
    uint8_t c;
    static char line[128];
    static int i = 0;

    while (uart_poll_in(uart_dev, &c) == 0) {
 
        /*if (c == '\n' || c == '\r') {
            if (i > 0) {
                line[i] = '\0';
                printk("Joystick: %s\n", line);
                i = 0;
            }
        } else {
            if (i < sizeof(line) - 1) {
                line[i++] = c;
            } else {
                i = 0;
                printk("Joystick: %s\n", line);
            }
        }
    }
    k_msleep(1);
}*/
 


        if (c == '\n' || c == '\r') {
            if (i > 0) {
                line[i] = '\0';

                // 🔥 KONVERTER TIL INT
                int val = atoi(line);

                if (val > 127) val = 127;
                if (val < -128) val = -128;

                // 🔥 SEND VIDERE TIL SYSTEMET
                gamepad.ly = (int8_t)val;

                LOG_INF("LY = %d", gamepad.ly);

                i = 0;
            }

        } else {
            if (i < sizeof(line) - 1) {
                line[i++] = c;
            } else {
                i = 0;
            }
        }
    }
}