//********************************************************************************************************
// Filename		: main.c
// Date			: April 2026
// Description	: Microbit code
// Path			: C:\Baldr_software\Kode\zephyrproject
// Compile		: west build -b bbc_microbit_v2 -p always my_apps/baldr_app --pristine
//********************************************************************************************************
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/drivers/can.h>
#include <stdio.h> //for tilgang til printf()
#include <stdint.h>  //for å bruke faste heltall
#include <stddef.h>  //tilgang til NULL og size_t
#include <math.h>
#include "joystick.h"
#include "motorstyring.h"
#include "gamepad_uart.h" //"8bitdo.h"
#include "canbus.h"

extern gamepad_t gamepad;

LOG_MODULE_REGISTER(main);

// Global variabel for 8BitDo-kontrolleren
//gamepad_t gamepad;

// Sikre at man får en fast sample-rate på f.eks 10 ms = 0.01 sec (tilsvarer 100 Hz)
K_TIMER_DEFINE(my_timer, NULL, NULL);

//****************************************************************************************************
// Hovedprogrammet
//****************************************************************************************************
int main(void)
{
    int 				err = 0;
	int					ret = 0;
	double 				dSampleFreq = 1000.0; // Hz
	double 				dTs = 0.0;
	double 				dTs_ms = 0.0;
	uint32_t 			nUpTime = 0;
    CarMovementInput_t  CarMovement;        // Bevegelser av bilen
    MotorVerdier_t      Motorverdier;       // Beregnede motorverider
	RPMVerdier_t 		RPMVerdier;
	double 				dScalingFactor = 1.0;
	int 				nfart = 0;
	int 				nfaktor = 0;
	
	dTs = 1.0/dSampleFreq;
	dTs_ms = (1000.0*dTs);

	// Initialisering av gamepad-data
	gamepad.buttons = 0;
	gamepad.lx = 0;
	gamepad.rx = 0;
	gamepad.ly = 0;
	gamepad.ry = 0;

    CarMovement.fart = 0.0;
    CarMovement.vinkel = 0.0;

	//****************************************************************************************************
	// Koble opp CAN-bussen
	//****************************************************************************************************
    if (!device_is_ready(can_dev)) {
        LOG_ERR("CAN device not ready\n");
        return -1;
    }

    ret = can_start(can_dev);
    if (ret != 0) {
        LOG_ERR("Failed to start CAN: %d\n", ret);
        return -1; 
    }
    LOG_INF("CAN started\n");
	
	//****************************************************************************************************
	// Her kobler man til 8BitDo-kontrolleren.
	//****************************************************************************************************
    /*LOG_INF("Starting BLE");
    err = bt_enable(NULL);
    if (err) {
        LOG_ERR("bt_enable failed (%d)", err);
        return 1;
    }

    LOG_INF("Scanning for 8BitDo controller…");
    err = bt_le_scan_start(BT_LE_SCAN_ACTIVE, device_found);
    if (err) {
        LOG_ERR("bt_le_scan_start failed (%d)", err);
        return 1;
    }
	*/

	gamepad_uart_init();

	// Kjøre appen i fast sample-rate
	k_timer_start(&my_timer, K_MSEC(dTs_ms), K_MSEC(dTs_ms)); // 	K_MSEC(ms)

	//****************************************************************************************************
    // Hovedløkka som håndterer styrer hele programmet fra Game-pad input til VESC controller.
	//****************************************************************************************************
    while (1) {
        gamepad_uart_update();
		
		k_timer_status_sync(&my_timer);
		nUpTime = k_uptime_seconds();

		// Gjør om fra Joystick/gamepad til Bil-verdier. joystick.c,h
		// Gir inn verdiene som trykker på Game-padden og regner om til hvordan hjulene skal kjøres
        //gamepad.buttons = 0x0008; //y-knapp aktivert
		//gamepad.lx = 0;
		//gamepad.ly = -25;
		//gamepad.rx = 0;
		//gamepad.ry = 0;
		HandleJoystick(gamepad, &CarMovement);
		//nfart = (int)(CarMovement.fart*100.0);
		//nfaktor = (int)(CarMovement.dScalingFactor*100.0);
		//LOG_INF("fart=%d, faktor=%d\n", nfart, nfaktor);
		// Gjør om fra Bil-verdier til Motor-verdier. motorstyring.c,h
		// Regner ut fra hjulenes bevegelse til verdier pr. hjul oppgitt i {-1.0..1.0}
        //CarMovement.fart = -0.1;
		//CarMovement.vinkel = 0.0;
		//CarMovement.rotasjon = 0.0;
		//CarMovement.dScalingFactor = 0.2;
		kalkulerMotorVerdier(CarMovement, &Motorverdier);
		
        // Send de fire Motor-verdiene (RPM-verdier) til VESC servo kontrollerne over CAN-bus.
		//Motorverdier.foran_venstre = 0.2;
		//dScalingFactor = 0.1; // Dene bør styres fra Joystick.
		dScalingFactor = CarMovement.dScalingFactor;
        kalkulerRPMVerdier(Motorverdier, &RPMVerdier, dScalingFactor);
		//RPMVerdier.foran_venstre_RPM = 100;
		ret = vesc_set_rpm(RPMVerdier.foran_venstre_RPM, VESC_ID1);			// Send to VESC controller 1
		ret = vesc_set_rpm(RPMVerdier.foran_hoyre_RPM, VESC_ID2);			// Send to VESC controller 2
		//ret = vesc_set_rpm(RPMVerdier.bak_venstre_RPM, VESC_ID3);			// Send to VESC controller 3
		//ret = vesc_set_rpm(RPMVerdier.bak_hoyre_RPM, VESC_ID4);			// Send to VESC controller 4

		if (ret == -EAGAIN) {
			// Queue full: ikke send mer nå
			// vent litt lenger før neste forsøk
			LOG_INF("CAN queue full, try again: %d\n", ret);
			k_sleep(K_MSEC(50));
		} else if (ret != 0) {
			LOG_INF("CAN error: %d\n", ret);
		}

        // Utskrift hvert sample
        //LOG_INF("RT-task, Ts=%f, ClockTime=%d\n", dTs_ms, nUpTime);
    }
	
	return 0;

	/*while (1) {

    	// 🔥 Les UART HELE tiden
    	gamepad_uart_update();

    	// 🔥 Kjør resten i fast rate
    	if (k_timer_status_get(&my_timer)) {

        	nUpTime = k_uptime_seconds();

        	HandleJoystick(gamepad, &CarMovement);

        	kalkulerMotorVerdier(CarMovement, &Motorverdier);

        	dScalingFactor = CarMovement.dScalingFactor;

        	kalkulerRPMVerdier(Motorverdier, &RPMVerdier, dScalingFactor);

        	ret = vesc_set_rpm(RPMVerdier.foran_venstre_RPM, VESC_ID1);

        	if (ret == -EAGAIN) {
            	LOG_INF("CAN queue full");
        	} else if (ret != 0) {
            	LOG_INF("CAN error: %d", ret);
        	}

        	LOG_INF("RT-task, Ts=%f, ClockTime=%d\n", dTs_ms, nUpTime);
    	}
	}*/
}


