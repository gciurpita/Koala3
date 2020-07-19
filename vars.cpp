// definitions for all global variables

#include "Arduino.h"

#include "vars.h"

const char *version   = "200719c";

// -----------------------------------------------------------------------------
//  dynamic variables

int      brake;
int      brakeInd;
int      brakePct = 123;
int      brakeCfm;

int      cars;
int      carLen = 40;

int      dir;
int      dirLst;

int      grX10    = 0;

int      engine   = 1;
int      loco     = 0;

int      mph;

int      timeSec;

int      mass;
int      reverser;
int      state;

int      throttle;
int      tonnage  = 0;

int      tractEff;
int      tractEffMax = 20000;

int      wtCar  = 80000;
int      wtLoco = 68000;    // NA 34T

Vars_s vars [] = {
    { & brake,    "brake" },
    { & brakeInd, "brakeInd" },
    { & brakePct, "brakePct" },
    { & brakeCfm, "brakeCfm" },
    { & cars,     "cars" },
    { & carLen,   "carLen" },
    { & dir,      "dir" },
    { & dirLst,   "dirLst" },
    { & loco,     "loco" },
    { & mph,      "mph" },
    { & mass,     "mass" },
    { & reverser, "reverser" },
    { & state,    "state" },
    { & throttle, "throttle" },
    { & tractEff, "tractEff" },
    { & tractEffMax, "tractEffMax" },
    { & timeSec,  "timeSec" },
    { & tonnage,  "tonnage" },
    { & wtCar,    "wtCar" },
    { & wtLoco,   "wtLoco" },
    { 0,        NULL },
};

Vars_s *pVars = & vars [0];

// -----------------------------------------------------------------------------
//  stored variables
int      adr [MAX_ADR]  = { 123, 456, 789, 400, 500, 600, 700, 800, 900 };

// WiFi and JMRI Server Definitions
#if 0
char     ssid [MAX_CHAR] = "WiFi-ssid";
char     pass [MAX_CHAR] = "WiFi-password";

char     host [MAX_CHAR] = "192.168.1.100";
int      port            = 12080;

#else
char     ssid [MAX_CHAR] = "FiOS-DGHZ0";
char     pass [MAX_CHAR] = "panorama123";

char     host [MAX_CHAR] = "192.168.1.174";
int      port            = 12080;
#endif

// -----------------------------------------------------------------------------
//  stored variables

EeVar_t eeVars [] = {
    { (void*)   ssid,     sizeof(ssid), V_STR, "ssid"},
    { (void*)   pass,     sizeof(pass), V_STR, "password" },
    { (void*)   host,     sizeof(host), V_STR, "hostname" },
    { (void*) & port,     sizeof(port), V_INT, "port" },

    { (void*) & adr [0],  sizeof(int),  V_INT, "addr_0" },
    { (void*) & adr [1],  sizeof(int),  V_INT, "addr_1" },
    { (void*) & adr [2],  sizeof(int),  V_INT, "addr_2" },

    { (void*) & adr [3],  sizeof(int),  V_INT, "addr_3" },
    { (void*) & adr [4],  sizeof(int),  V_INT, "addr_4" },
    { (void*) & adr [5],  sizeof(int),  V_INT, "addr_5" },
    { (void*) & adr [6],  sizeof(int),  V_INT, "addr_6" },

    { (void*) & adr [7],  sizeof(int),  V_INT, "addr_7" },
    { (void*) & adr [8],  sizeof(int),  V_INT, "addr_8" },
    { (void*) & adr [9],  sizeof(int),  V_INT, "addr_9" },

    { NULL,               0,            V_NUL, NULL     },
};

EeVar_t *pEeVars = & eeVars [0];
