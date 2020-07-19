#ifndef VARS_H
# define VARS_H

#include <stdio.h>

typedef unsigned char byte;

#define MAX_CHAR  33

#define MAX_BRK   5
#define MAX_THR   100
#define MAX_REV   100
#define MAX_MID   (MAX_REV / 2)

enum { DIR_NEUTRAL, DIR_FOR, DIR_REV };
enum { V_NUL, V_STR, V_INT };
enum { BRK_REL, BRK_LAP, BRK_SVC1, BRK_SVC2, BRK_SVC3, BRK_EMER };

struct Vars_s {
    int        *p;
    const char *desc;
};

extern Vars_s  *pVars;

typedef struct {
    void       *p;
    byte        nByte;
    byte        type;
    const char *desc;
} EeVar_t;

extern EeVar_t *pEeVars;

// -------------------------------------
// dynamic variables

extern int      brake;
extern int      brakeInd;
extern int      brakePct;
extern int      brakeCfm;

extern int      cars;
extern int      carLen;

extern int      dir;
extern int      dirLst;

extern int      grX10;

extern int      engine;
extern int      loco;

extern int      mph;

extern int      mass;
extern int      reverser;
extern int      state;

extern int      throttle;
extern int      timeSec;

extern int      tractEff;
extern int      tractEffMax;

extern int      wtCar;
extern int      wtLoco;

// -------------------------------------
// stored variables
#define MAX_ADR  10
extern int      adr [MAX_ADR];

extern char     host [];
extern int      port;

extern char     ssid [];
extern char     pass [];

extern const char *version;

#endif
