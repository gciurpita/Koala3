// physics models

#include "Arduino.h"

#include "brakes.h"
#include "koala.h"
#include "phyConst.h"
#include "physics.h"
#include "rollRes.h"
#include "vars.h"

unsigned long msecLst = 0;
int           dMsec;

// ---------------------------------------------------------
void _reverser (void)
{
    if (DIR_FOR != dir && (MAX_MID + 10) < reverser)  {
        dir = DIR_FOR;
        wifiSend ("TR1");
        printf ("%s: For\n", __func__);
    }
    else if (DIR_REV != dir && (MAX_MID - 10) > reverser)  {
        dir = DIR_REV;
        wifiSend ("TR0");
        printf ("%s: Rev\n", __func__);
    }
    else if (DIR_NEUTRAL != dir &&
        (MAX_MID - 10) < reverser && reverser < (MAX_MID + 10))  {
        dir      = DIR_NEUTRAL;
        throttle = 0;
        printf ("%s: Neutral\n", __func__);
    }
}

// ---------------------------------------------------------
void _throttle (void)
{
    tractEff = tractEffMax * throttle / MAX_THR;
}

// -----------------------------------------------------------------------------
#define G   32.2        // gravitation acceleration ft/sec/sec

float acc;
float force;
float fps = 0;          // should be initialized when loco set

int   hdr = 0;
int   mphLst = 0;
float whRes;
int   wtTot;

// ---------------------------------------------------------
void physics (
    unsigned long msec)
{
    if (0)
        Serial.println (__func__);

    if (0 == msecLst)  {
        msecLst = msec;
        Serial.println ("physics -  init msecLst");
        return;
    }

    dMsec   = msec - msecLst;

    _reverser ();
    brakes (dMsec);
    _throttle ();

    wtTot = wtLoco + (cars * wtCar);
    mass  = wtTot / G;

    // forces
    float rf   =  rollRes (fps/MphTfps);    // wheel resistance
    float tons =  wtTot / LbPton;
    whRes  = rf * tons;

    float grF  = wtTot * grX10 / 1000;      // slope
#define NBR     0.10        // nominal brake ratio
    float brkF = (cars * wtCar) * NBR * brakePct / 100;

    force  = tractEff;      // tractive effort
    force -= whRes;         // wheel bearing resistance
    force -= grF;           // grade

    if (0.05 < fps)
        force -= brkF;

    // acceleraton
    acc   = force / mass;
    fps  += acc * dMsec / 1000;
    mph   = int (fps / MphTfps);

    // display values
    static int secLst = 0;
    int sec = (msec / 1000) % 60;
    int min = msec / 60000;

    if (secLst != sec)  {
        secLst = sec;
    
        if (! (hdr++ % 10))  {
            printf ("%6s", "");

#undef CONST
#ifdef CONST
            printf (" %8s %8s %8s %8s",
                "wtLoco", "wtCar", "wtTot", "mass");
#endif

            printf (" %4s %3s %6s %6s %6s %6s %8s %6s %6s %3s", "cars", "thr", 
                "TE", "res", "grF", "brF", "force", "acc", "fps", "mph");

            printf (" %4s %4s %6s %6s %6s %5s",
                "", "vol", "flRat", "fil", "psi", "pct");
            printf ("\n");
        }

        printf (" %2d:%02d", min, sec);

#ifdef CONST
        printf (" %8d", wtLoco);
        printf (" %8d", wtCar);
        printf (" %8d", wtTot);
        printf (" %8d", mass);
#endif

        printf (" %4d", cars);
        printf (" %3d", throttle);
        printf (" %6d", tractEff);
        printf (" %6.0f", whRes);
        printf (" %6.1f", grF);
        printf (" %6.0f", brkF);
        printf (" %8.0f", force);
        printf (" %6.2f", acc);
        printf (" %6.1f", fps);
        printf (" %3d", mph);

        brakesPr ();
        printf ("\n");
    }

    // update JMRI
    if (mphLst != mph)  {
        sprintf (s, "TV%d", mphLst = mph);
        wifiSend (s);
    }

    msecLst = msec;
}
