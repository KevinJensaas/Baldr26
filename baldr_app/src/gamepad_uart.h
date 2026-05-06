#ifndef _GAMEPAD_UART_H_
#define _GAMEPAD_UART_H_

#include "joystick.h"

extern gamepad_t gamepad;

void gamepad_uart_init(void);
void gamepad_uart_update(void);

#endif