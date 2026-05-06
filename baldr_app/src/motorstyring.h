//********************************************************************************************************
// Filename		: H-file
// Description	: Handling Motorstyring
//********************************************************************************************************
#ifndef _MOTORTSTYRING_H_INCLUDED	/* Allow multiple inclusions										 */
#define _MOTORTSTYRING_H_INCLUDED

#include <zephyr/logging/log.h>
#include "joystick.h"

#define MAX_RPM		4000
#define MAX_ERPM	(7*MAX_RPM) //Elektrisk RPM = 7 polpar * RPM

// Struct som holder output-verdier for de fire hjulene. Verdiene er oppgit {-1.0..1.0}
// Verdiene må regnes om til RPM
typedef struct  {
    double  foran_venstre;
    double  foran_hoyre;
    double  bak_venstre;
    double  bak_hoyre;
} MotorVerdier_t;

typedef struct  {
    double  foran_venstre_RPM;
    double  foran_hoyre_RPM;
    double  bak_venstre_RPM;
    double  bak_hoyre_RPM;
} RPMVerdier_t;

//********************************************************************************************************
int kalkulerMotorVerdier(CarMovementInput_t CarMovement, MotorVerdier_t *pMotorverdier);
int kalkulerRPMVerdier(MotorVerdier_t Motorverdier, RPMVerdier_t *pRPMVerdier, double dScalingFactor);

#endif /* _MOTORSTYRING_H_INCLUDED																		   */
