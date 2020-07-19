#include <Arduino.h>

#include "brakes.h"
#include "vars.h"

// ---------------------------------------------------------
// train brakes
#define AtmPsi      14.7
#define BrkLnDia    1.5
#define BrkLnRad    (BrkLnDia/2)
#define BrkLnArea   (PI * BrkLnRad * BrkLnRad)

float brkLnFil = 0;
float brkLnPsi = 0;
float brkFlRat = 0;
float brkLnVol;

void _trainBrakes (
    int dMsec )
{ 
#define SqFt    144
    brkLnVol = cars * carLen * BrkLnArea / SqFt;
    brkFlRat = 0;

    switch (brake)  {
    case BRK_REL:
#define FILL_RATE   (40.0/60)      // cu.ft. / min
        brkFlRat = FILL_RATE;
        brakePct = 0;
        break;

    case BRK_SVC3:
    case BRK_SVC2:
    case BRK_SVC1:
#define SVC_RATE    (50.0/60)      // cu.ft. / min
        brkFlRat = -SVC_RATE;
        break;

    case BRK_EMER:
#define EMER_RATE   (90.0/60)      // cu.ft. / min
        brkFlRat = -EMER_RATE;
        break;

    case BRK_LAP:
    default:
        break;
    }

    // update brake line fill
#define BRK_PSI_MAX  90
    if (   (0 < brkFlRat && BRK_PSI_MAX >= brkLnPsi)
        || (0 > brkFlRat && 0 < brkLnPsi) )  {

        brkLnFil  = brkLnVol * brkLnPsi / AtmPsi;
        brkLnFil += brkFlRat * dMsec / 1000;
    }

    // update brake line pressure
    brkLnPsi  = AtmPsi * brkLnFil / brkLnVol;

    // update braking %
#define BRK_PCT_COEF    3
    if (BRK_REL != brake)  {
        brakePct = BRK_PCT_COEF * (BRK_PSI_MAX - brkLnPsi);
        brakePct = 100 < brakePct ? 100 : brakePct;
    }
}

// ---------------------------------------------------------
// independent brake routine


// ---------------------------------------------------------
const char *brkStr [] = { "Rel", "Lap", "Svc1", "Svc2", "Svc3", "Emer" };

void
brakesPr (void)
{
    printf (" %4s",   brkStr [brake]);
    printf (" %4.1f", brkLnVol);
    printf (" %6.2f", brkFlRat);
    printf (" %6.1f", brkLnFil);
    printf (" %6.1f", brkLnPsi);
    printf (" %3d %%", brakePct);
}

// ---------------------------------------------------------
void
brakes (
    int dMsec )
{
    _trainBrakes (dMsec);
}
