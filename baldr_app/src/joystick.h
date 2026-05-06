//********************************************************************************************************
// Filename		: H-file
// Description	: Handling Joystick
//********************************************************************************************************
#ifndef _JOYSTICK_H_INCLUDED	/* Allow multiple inclusions											    */
#define _JOYSTICK_H_INCLUDED

#include <zephyr/logging/log.h>

#define MAX_VALUE(A,B) ((A)  > (B) ? (A) : (B))
#define MIN_VALUE(A,B) ((A)  < (B) ? (A) : (B))

#define BUTTON_A			0x0001	// bit 0
#define BUTTON_B			0x0002	// bit 1
#define BUTTON_X			0x0004	// bit 2
#define BUTTON_Y			0x0008	// bit 3

#define BUTTON_L1			0x0010	// bit 4
#define BUTTON_R1			0x0020	// bit 5
#define BUTTON_Select		0x0040	// bit 6
#define BUTTON_Start		0x0080	// bit 7

#define BUTTON_DPAD_UP		0x0100	// bit 8
#define BUTTON_DPAD_DOWN	0x0200	// bit 9
#define BUTTON_DPAD_LEFT	0x0400	// bit 10
#define BUTTON_DPAD_RIGHT	0x0800	// bit 11

#define BUTTON_L3			0x1000	// bit 12
#define BUTTON_R3			0x2000	// bit 13

/*********************************************************************************************************
 Parsed controller state 
-128  = full venstre / full opp
  0   = midten
+127  = full høyre / full ned
bit 0  = A
bit 1  = B
bit 2  = X
bit 3  = Y
bit 4  = L1
bit 5  = R1
bit 6  = Select
bit 7  = Start
bit 8  = D-pad Up
bit 9  = D-pad Down
bit 10 = D-pad Left
bit 11 = D-pad Right
bit 12 = L3 (left stick button)
bit 13 = R3 (right stick button)
*********************************************************************************************************/
typedef struct {
    uint16_t buttons;	// Alle knappene
    int8_t lx;			// Left joystick (x-retning)
    int8_t ly;			// Left joystick (y-retning)
    int8_t rx;			// Right joystick (x-retning)
    int8_t ry;			// Right joystick (y-retning)
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
int beregn_fart_og_vinkel(double x, double y, double *fart, double *vinkel);
int skaler_fart(double fart, int profil, double *skalert_fart);
int HandleJoystick(gamepad_t gamepad, CarMovementInput_t* pCarMovement);

#endif /* _JOYSTICK_H_INCLUDED																			*/
