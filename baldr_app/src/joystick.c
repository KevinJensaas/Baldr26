//********************************************************************************************************
// Filename		: joystick.c
// Description	: Program taking Joystick input as speed reference and limits the accelaration
//********************************************************************************************************
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "joystick.h"

LOG_MODULE_REGISTER(joystick);

const double pi = 3.14159265358979;

//********************************************************************************************************
int beregn_bevegelse(double lx, double ly, double rx, double *fart, double *vinkel, double *rotasjon)
{
    //# dødsone – ignorer små bevegelser
    if (fabs(lx) < 0.1) {
        lx = 0.0;
	}
	
    if (fabs(ly) < 0.1) {
        ly = 0.0;
	}

	if (fabs(rx) < 0.1) {
        rx = 0.0;
    }

	//fart
	
    *fart = sqrt(lx*lx + ly*ly);
    *fart = MIN_VALUE(*fart, 1.0);
	*fart = MAX_VALUE(*fart, -1.0);

    if (*fart < 0.1) {
        *fart = 0.0;
	}
	
	//vinkel

    //# Vinkel: 0° = opp, 90° = høyre, 180° = ned, 270° = venstre
    *vinkel = atan2(lx, -ly);  //# OBS: bytt rekkefølge og tegn
    if (*vinkel < 0.0) {
        *vinkel += 2.0 * pi;
	}

	//rotasjon

	*rotasjon = rx;

    *rotasjon = MIN_VALUE(*rotasjon, 1.0);
    *rotasjon = MAX_VALUE(*rotasjon, -1.0);

    if (fabs(*rotasjon) < 0.1) {
        *rotasjon = 0.0;
    }
	
    return 0;
}

//# Bestem fart basert på valgt profil
//********************************************************************************************************
int skaler_bevegelse(double fart, double rotasjon, int profil, double *skalert_fart, double *skalert_rotasjon)
{
    if (profil == 1) {
        *skalert_fart = fart * 0.2;
		*skalert_rotasjon = rotasjon * 0.2;
    } else if (profil == 2) {
        *skalert_fart = (fart * 0.4);
		*skalert_rotasjon = rotasjon * 0.2;
    } else if (profil == 3) {
        *skalert_fart = (fart * 0.8);
		*skalert_rotasjon = rotasjon * 0.2;
	} else if (profil == 4) {
        *skalert_fart = (fart * 1.0);
		*skalert_rotasjon = rotasjon * 0.2;
    }
	
	return 0;
}

//********************************************************************************************************
int HandleJoystick(gamepad_t gamepad, CarMovementInput_t* pCarMovement)
{
	double 	rotasjon = 0.0;
	int 	aKnapp = 0;
	int 	bKnapp = 0;
	int 	yKnapp = 0;
	int 	xKnapp = 0;
	double 	venstre_x = 0.0;
	double 	venstre_y = 0.0;
	double 	hoyre_x = 0.0;
	static int 	Hastighetsmodus = 1;
	int 	Ret = 0;
	double 	fart = 0.0;
	double 	vinkel = 0.0;
	
	// === Input Knapper ===
	aKnapp = gamepad.buttons & BUTTON_A;  	//# A-knapp
	bKnapp = gamepad.buttons & BUTTON_B;  	//# B-knapp
	yKnapp = gamepad.buttons & BUTTON_Y;  	//# Y-knapp
	xKnapp = gamepad.buttons & BUTTON_X;  	//# X-knapp

	//# == Input Joystick ===
	venstre_x = (double)gamepad.joystick[0]*(2.0/1023.0) - 1.0; //Gjør om fra {0, 1023} til {-1.0, 1.0}
	venstre_y = (double)gamepad.joystick[1]*(2.0/1023.0) - 1.0; //Gjør om fra {0, 1023} til {-1.0, 1.0}
	hoyre_x = (double)gamepad.joystick[2]*(2.0/1023.0) - 1.0; //Gjør om fra {0, 1023} til {-1.0, 1.0}

	pCarMovement->dScalingFactor = 0.2; //1.0

	//# === Endre hastighetsmoduser ===
	/* Endre modus */

if (bKnapp) {

    Hastighetsmodus = 1;

} else if (aKnapp) {

    Hastighetsmodus = 2;

} else if (yKnapp) {

    Hastighetsmodus = 3;

} else if (xKnapp) {

    Hastighetsmodus = 4;
}

/* Bruk lagret modus */

switch (Hastighetsmodus) {

case 1:
    pCarMovement->dScalingFactor = 0.2;
    break;

case 2:
    pCarMovement->dScalingFactor = 0.4;
    break;

case 3:
    pCarMovement->dScalingFactor = 0.8;
    break;

case 4:
    pCarMovement->dScalingFactor = 1.0;
    break;

default:
    pCarMovement->dScalingFactor = 0.2;
    break;
}

	//# Beregn fart og vinkel
	Ret = beregn_bevegelse(venstre_x, venstre_y, hoyre_x, &fart, &vinkel, &rotasjon);

	pCarMovement->fart = fart;
	pCarMovement->vinkel = vinkel;
	pCarMovement->rotasjon = rotasjon;

    return 0;
}
