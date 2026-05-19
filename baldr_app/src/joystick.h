//********************************************************************************************************
// Filename		: H-file
// Description	: Handling Joystick
//********************************************************************************************************
#ifndef _JOYSTICK_H_INCLUDED	/* Allow multiple inclusions											    */
#define _JOYSTICK_H_INCLUDED

#include <zephyr/logging/log.h>

#define MAX_VALUE(A,B) ((A)  > (B) ? (A) : (B))
#define MIN_VALUE(A,B) ((A)  < (B) ? (A) : (B))

#define BUTTON_B			0x0001	// bit 0
#define BUTTON_A			0x0002	// bit 1
#define BUTTON_Y			0x0004	// bit 2
#define BUTTON_X			0x0008	// bit 3
#define BUTTON_RT			0x0080  // bit 7

#define MAX_JOYSTICK_AXES       3
#define BUTTONS_AXES            1

typedef struct {
    uint16_t buttons;	    // Alle knappene
    uint16_t joystick[MAX_JOYSTICK_AXES];	// Joysticks, left and right (lx, ly, rx)
} gamepad_t;

typedef struct {
	//Fart og rotasjon
    //hvor fort bilen skal bevege seg. 
    // 0.0 = full stopp
    // 1.0 = maks fart 
    double fart;

    //hvilken retning bilen skal bevege seg i (0 til 2pi radianer) 
    // 0 = rett frem
    // pi/2 = mot venstre
    // pi =  bakover
    // 3pi/2 =  mot høyre
    double vinkel;

    //Den rotasjonen vi ønsker rundt egen akse
    // -1.0 = roter mot venstre
    // 0.0 = ingen rotasjon
    // 1.0 = roter mot høyre
    double rotasjon;

    double dScalingFactor;

    //ekstra finjustering for sving fra en egen styrespak, for at bilen skal svinge som en ekte bil.
    //double sving_js)
} CarMovementInput_t;


//********************************************************************************************************
int beregn_bevegelse(double lx, double ly, double rx, double *fart, double *vinkel, double *rotasjon);
int skaler_bevegelse(double fart, double rotasjon, int profil, double *skalert_fart, double *skalert_rotasjon);
int HandleJoystick(gamepad_t gamepad, CarMovementInput_t* pCarMovement);

#endif /* _JOYSTICK_H_INCLUDED																			*/
