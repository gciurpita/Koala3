// train rolling resistance

#include <stdio.h>

#include "rollRes.h"

// ----------------------------------------------------------
// train resistance (lb/ton) tables from Armstrong

float resMph   [] = { 0, 10,   20,   30,   40,   50,   60,   70, };
float resEmpty [] = { 0, 4.5,  5.4,  6.7,  9.2, 11.8, 14.8, 19.8, };
float resFull  [] = { 0, 2.3,  3.0,  3.9,  5.2,  6.6,  8.4, 10.4, };
float resMix   [] = { 0, 3.4,  4.2,  5.3,  7.2,  9.2, 11.6, 15.2, };

#define RES  (sizeof(resFull) / sizeof(float))

// ----------------------------------------------------------
// interpolate - 
static float
interp (
    float   x, 
    float   vecX [], 
    float   vecY [], 
    int     N )
{
    // find vecX seqment x is on
    int  n;
    for (n = 1; n < N; n++)  {
        if (vecX [n] > x)
            break;
    }

#if 1
    if (N == n)
        n--;
#endif

    // calc y on corresponding seqment of vecY
    float k   = (x - vecX [n-1]) / (vecX [n] - vecX [n-1]);
    float res = vecY [n-1] + k * (vecY [n] - vecY [n-1]);

#if 0
        printf (" %s:", __func__);
        printf (" %2d", n);
        printf (" %5.1f", x);
        printf (" %5.1f", vecX [n-1]);
        printf (" %5.1f", vecX [n]);
        printf (" %5.1f", k);
        printf (" %5.1f", vecY [n-1]);
        printf (" %5.1f", vecY [n]);
        printf (" %5.1f", res);
        printf ("\n");
#endif

    return res;
}

// ----------------------------------------------------------
// train resistance (lb/ton) using resFull [] or refEmpty []
float
rollResMtFull (
    float mph,
    float vec [])
{
    if (0 == mph)
        mph = 10;
    return interp (mph, resMph, vec, RES);
}


// ----------------------------------------------------------
// train resistance (lb/ton) using resFull [] or refEmpty []
float
rollRes (
    float mph )
{
    float res = interp (mph, resMph, resMix, RES);

    res = 0 > res ? 0 : res;

    return res;
}
