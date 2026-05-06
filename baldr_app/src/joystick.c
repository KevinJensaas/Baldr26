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
int beregn_fart_og_vinkel(double x, double y, double *fart, double *vinkel)
{
    //# dødsone – ignorer små bevegelser
    if (fabs(x) < 0.1) {
        x = 0.0;
	}
	
    if (fabs(y) < 0.1) {
        y = 0.0;
	}
	
    *fart = sqrt(x*x + y*y);
    *fart = MIN_VALUE(*fart, 1.0);
	*fart = MAX_VALUE(*fart, -1.0);

    if (*fart < 0.1) {
        *fart = 0.0;
	}
	
    //# Vinkel: 0° = opp, 90° = høyre, 180° = ned, 270° = venstre
    *vinkel = atan2(x, -y);  //# OBS: bytt rekkefølge og tegn
    if (*vinkel < 0.0) {
        *vinkel += 2.0 * pi;
	}
	
    return 0;
}

//# Bestem fart basert på valgt profil
//********************************************************************************************************
int skaler_fart(double fart, int profil, double *skalert_fart)
{
    if (profil == 1) {
        *skalert_fart = fart * 0.3;
    } else if (profil == 2) {
        *skalert_fart = (fart * 0.3);
    } else if (profil == 3) {
        *skalert_fart = (fart * 0.6);
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
	int 	rb = 0;
	int 	lb = 0;
	int 	rt = 0;	
	int 	lt = 0;	
	double 	venstre_x = 0.0;
	double 	venstre_y = 0.0;
	int 	Hastighetsmodus = 0;
	double 	rotasjon_faktor = 0.1;
	int 	Ret = 0;
	double 	fart = 0.0;
	double 	vinkel = 0.0;
	
	// === Input Knapper ===
	aKnapp = gamepad.buttons & BUTTON_A;  	//# A-knapp
	bKnapp = gamepad.buttons & BUTTON_B;  	//# B-knapp
	yKnapp = gamepad.buttons & BUTTON_Y;  	//# Y-knapp

	//# === Input knapper for rotasjon LB og RB ===
	rb = gamepad.buttons & BUTTON_R3;  //# Høyre bumper
	lb = gamepad.buttons & BUTTON_L3;  //# Venstre bumper

	if (rb == 1) {
		rotasjon = 1.0;
	} else if (lb == 1) {
		rotasjon = -1.0;
	}

	//# == Input Joystick ===
	venstre_x = (double)gamepad.lx/128.0; //Gjør om fra {-128, 127} til {-1.0, 1.0}
	venstre_y = (double)gamepad.ly/128.0; //Gjør om fra {-128, 127} til {-1.0, 1.0}

	//# === Input Trigger RT og LT ===
	rt = gamepad.buttons;  //# Høyre trigger (RT)
	lt = gamepad.buttons;  //# Venstre trigger (LT)

	//# Håndter rotasjon (RB og LB)
	//# Bestem rotasjon basert på bumpere

	pCarMovement->dScalingFactor = 0.2; //1.0

	//# === Endre hastighetsmoduser ===
	if (yKnapp) {
		Hastighetsmodus = 1; 
		pCarMovement->dScalingFactor = 0.1;
	} else if (bKnapp) {
		Hastighetsmodus = 2;
		pCarMovement->dScalingFactor = 0.5;
	} else if (aKnapp) {
		Hastighetsmodus = 3;
		pCarMovement->dScalingFactor = 0.7;
	}
	
	//# Velg rotasjonsfart som faktor
	//rotasjon_faktor = {1: 0.2, 2: 0.5, 3: 0.7}.get(Hastighetsmodus, 0.2)
	rotasjon_faktor = 0.2;

	//# Beregn fart og vinkel
	Ret = beregn_fart_og_vinkel(venstre_x, venstre_y, &fart, &vinkel);

	pCarMovement->fart = fart;
	pCarMovement->vinkel = vinkel;
	pCarMovement->rotasjon = rotasjon;

    return 0;
}
