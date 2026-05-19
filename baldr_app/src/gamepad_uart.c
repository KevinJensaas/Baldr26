#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <stdio.h>

#include "gamepad_uart.h"
#include "joystick.h"

LOG_MODULE_REGISTER(gamepad_uart);

#define UART_NODE DT_NODELABEL(uart1)

static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

gamepad_t gamepad = {0};

int JS_decode(uint8_t c)
{
    int         nStatus     = 1;            // 1: OK, 0: No data, -1: Error
    static char line[MAX_NUM_DIGITS + 1];   // Buffer for å bygge opp tallstrengen
    static int  iCnt = 0;                   // Indeks for å holde styr på hvor vi er i linjen
    static int  comma_index = 0;            // Indeks for å holde styr på hvilken joystick-knapp vi er på

    // Sjekk om tegnet er et siffer (0-9)
    if ((c >= '0') && (c <= '9')) {
        if (iCnt <= MAX_NUM_DIGITS) {
            line[iCnt++] = c;
        }
    // Fortsett å lese til vi får et komma, som indikerer at vi har fullført en joystick-verdi
    } else if (c == ',') {
        if (iCnt > 0) {
            line[iCnt] = '\0';
            // Konverter linjen til et heltall
            int value = atoi(line);

            // Oppdater gamepad_values med den konverterte verdien
            gamepad.joystick[comma_index] = value;
            iCnt = 0;       // Tilbakestill for neste verdi
            comma_index++;  // Gå til neste joystick-knapp
        }
    // Når vi får linjeskift, da er siste verdi knappe-verdiene. Tilbakestill så for neste runde.
    } else if ((c == '\n') || (c == '\r')) {
        if (iCnt > 0) {
            line[iCnt] = '\0';
            int value = atoi(line);
            gamepad.buttons = value;
            iCnt = 0;           // Tilbakestill for neste runde
            comma_index = 0;    // Tilbakestill for neste runde
        }
    }
    return nStatus;
}

/* Interrupt callback */
void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t buf[UART_FIFO_SIZE];
    int length;

    /* Så lenge det finnes bytes i FIFO, les dem */
    while (uart_irq_update(dev) && uart_irq_rx_ready(dev)) {
        length = uart_fifo_read(dev, buf, sizeof(buf));

        for (int i = 0; i < length; i++) {
            uint8_t c = buf[i];
            /* Her prosesserer du hver byte */
            JS_decode(c);
        }
    }
}

//****************************************************************************************************
void gamepad_uart_init(void)
{
    if (!device_is_ready(uart_dev)) {
        LOG_ERR("UART not ready\n");
        return;
    }

    /* Sett callback */
    uart_irq_callback_set(uart_dev, uart_cb);

    /* Aktiver RX-interrupt */
    uart_irq_rx_enable(uart_dev);

    LOG_INF("Gamepad UART ready");
}