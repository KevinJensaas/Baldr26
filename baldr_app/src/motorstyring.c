//********************************************************************************************************
// Filename		: motorstyring.c
// Description	: Gjør om fra Joystick input til pådrag på alle fire hjulene
//********************************************************************************************************
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "motorstyring.h"

LOG_MODULE_REGISTER(Motorstyring);

//********************************************************************************************************
// Funksjon som regner ut hvor mye hver motor skal bidra med for å oppnå ønsket fart, vinkel, rotasjon og sving
//********************************************************************************************************
int kalkulerMotorVerdier(CarMovementInput_t CarMovement, MotorVerdier_t *pMotorverdier)
{
    double rotering = 0.0;
    double x = 0.0;
    double y = 0.0;

    // Hvis bilen står stille og RB/LB trykkes så vil bilen rotere med en fast hastighet.
    // trykkes en annen kjøreknapp så stoppes rotasjonen.
    if (CarMovement.fart == 0.0 && CarMovement.rotasjon != 0.0) {
        // bruker 0.3 fordi maks rpm er satt til 4tusen, ved å bruke 0.3 * 4tusen
        // så blir rotasjonsfarta 1,2tusen rpm, fast og kontrollert.
        if (CarMovement.rotasjon > 0) {
            rotering = 0.3;
        } else {
            rotering = -0.3;
        } 
    }
    
    // Regner ut hvor mye bevegelse som skal være i X-retning (sideveis)
    // Omgjør fart og vinkel til en x-verdi
    // sinf(vinkel) gir hvor mye som skal gå i X-retning
    x = CarMovement.fart * sin(CarMovement.vinkel);

    // Regner ut hvor mye bevegelse som skal være i Y-retning (fremover/bakover)
    // Omgjør fart og vinkel til en Y-verdi
    // cosf(vinkel) gir hvor mye som skal gå i Y-retning
    // X og Y tilsammen er en bevegelsesvektor som
    y = CarMovement.fart * cos(CarMovement.vinkel);

    // for å legge til ekstra sidebevegelse (sving) fra joystick for svinging
    // sving_js ganges med 0.5 for å unngå for brå/kraftig svingeffekt.
    // x += CarMovement.sving_js * 0.5;

    // for å regne ut hvor mye bidrag hvert hjul må tilføre for å kunne bevege fremover, bakover eller sideveis
    // verdiene y og x er fremdrift og sidebevegelse

    // foran venstre (kombinerer retning fremover og høyre bevegelse)
    pMotorverdier->foran_venstre = y + x;

    // foran høyre hjul (kombinerer retning fremover og venstre bevegelse)
    pMotorverdier->foran_hoyre = y - x;

    // bak_venstre (kombinerer retning bakover og venstre bevegelse)
    pMotorverdier->bak_venstre = y - x;

    // bak høyre (kombinerer retning bakover og høyre bevegelse)
    pMotorverdier->bak_hoyre = y + x;

    // Etter at det har blitt beregnet hvor mye hvert hjul må bidra med er det viktig å normalisere verdiene
    // normaliserer verdiene for å unngå at noen av verdiene overstiger -0.1 eller 0.1
    // overstiger de dette kan det føre til uforventet bevegelser

    // finner den største absoluttverdien av motorverdiene.
    // bruker funksjonen fmaxf som kan sammenligne to verdier om gangen
    // bruker fmaxf på de to hjulene foran for å finne den største verdien av de to hjulene foran
    // bruker fmaxf på de to hjulene bak for å finne den største verdien av de to hjulene bak
    // bruker deretter fmaxf på begge fmaxf verdiene for å finne hvilken verdi 
    // som er størst av den største foran og største bak
    // den største verdien av de fire legges deretter i "maxverdi"
    double maksverdi = fmaxf(   fmaxf(fabsf(pMotorverdier->foran_venstre), fabsf(pMotorverdier->foran_hoyre)),
                                fmaxf(fabsf(pMotorverdier->bak_venstre), fabsf(pMotorverdier->bak_hoyre)));

    // hvis maksverdi er større enn 1.0 må vi skalere og dette gjøres ved å dele alle 
    // hjulverdiene på maksverdien vi fant. 
    if (maksverdi > 1.0) {
        pMotorverdier->foran_venstre /= maksverdi;
        pMotorverdier->foran_hoyre /= maksverdi;
        pMotorverdier->bak_venstre /= maksverdi;
        pMotorverdier->bak_hoyre /= maksverdi;
    }
	
	return 0;
}

//********************************************************************************************************
// Regner om fra normalisert verdi for hastighet pr. hjul til RPM pr. hjul
//********************************************************************************************************
int kalkulerRPMVerdier(MotorVerdier_t Motorverdier, RPMVerdier_t *pRPMVerdier, double dScalingFactor)
{
	pRPMVerdier->foran_venstre_RPM = Motorverdier.foran_venstre * MAX_ERPM * dScalingFactor;
	pRPMVerdier->foran_hoyre_RPM = Motorverdier.foran_hoyre * MAX_ERPM * dScalingFactor;
	pRPMVerdier->bak_venstre_RPM = Motorverdier.bak_venstre * MAX_ERPM * dScalingFactor;
	pRPMVerdier->bak_hoyre_RPM = Motorverdier.bak_hoyre * MAX_ERPM * dScalingFactor;
	
	return 0;
}

