#ifndef _GAMEPAD_UART_H_
#define _GAMEPAD_UART_H_

#include "joystick.h"

#define UART_FIFO_SIZE  32

#define MAX_NUM_DIGITS          4
#define MAX_UART_DATA_LENGTH    35

extern gamepad_t gamepad;

void gamepad_uart_init(void);
void gamepad_uart_update(void);

#endif