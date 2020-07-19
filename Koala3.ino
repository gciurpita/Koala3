// Koala Throttle
// -----------------------------------------------------------------------------

#define BT
#ifdef BT
#include <BluetoothSerial.h>
#endif

#include <WiFi.h>

#include "Koala.h"
#include "file.h"
#include "koala.h"
#include "pcRead.h"
#include "physics.h"
#include "server.h"
#include "vars.h"

unsigned int debug = 1;

// -----------------------------------------------------------------------------
// Initialize the OLED display using Wire library

#ifdef SH1106
SH1106Wire  display(0x3c, 21, 22);
#else
SSD1306Wire  display(0x3c, 21, 22);
#endif

#ifdef BT
BluetoothSerial     serialBT;
#endif
WiFiClient          wifi;

#define SCREEN_WID  128
#define SCREEN_HT    64

// -------------------------------------
const char* project    = "Koala-3 Throttle";

//  Switch  assignment table:
#define  PB            26

#define  TA            2
#define  TB            4
#define  TC            5
#define  TD            12
#define  TE            13

#define  TF            14
#define  TG            15

#define  TH            26
#define  TI            27
#define  TJ            23

#define  ADC_REV       32   // Wiper of 10K Speed Pot Ends=GND & +3.3V
#define  ADC_THR       39   // Wiper of 10K Speed Pot Ends=GND & +3.3V
#define  ADC_MAX       4095

byte pinPullups [] = { TA, TB, TC, TD, TE, TF, TG, TH, TI, TJ };

#define N_BUTTONS   sizeof(pinPullups)

enum {
    BUT_LAMP, BUT_UP, BUT_DN, BUT_MENU, BUT_SEL, BUT_HORN,
    BUT_G, BUT_H, BUT_I, BUT_J
};

byte butState   [N_BUTTONS] = {};
byte butPress   [N_BUTTONS] = {};

// -----------------------------------------------------------------------------
#define ST_NULL     (0)
#define ST_WIFI     (1 << 0)
#define ST_JMRI     (1 << 1)
#define ST_NO_LOCO  (1 << 2)
#define ST_HORN     (1 << 3)
#define ST_MENU     (1 << 4)
#define ST_CFG      (1 << 5)

const char * stateStr [] = {
    "ST_WIFI",
    "ST_JMRI",
    "ST_NO_LOCO",
    "ST_HORN",
    "ST_MENU",
    "ST_CFG",
};
#define N_STATE_STR  (sizeof(stateStr)/sizeof(char *))

char s0[30];
char s1[30];
char s [80];

unsigned long msec;

// ---------------------------------------------------------
#define N_FUNC 29
int funcState [N_FUNC] = {};

int
wifiFuncKey (
    unsigned func,
    int       cmd )
{
    if (N_FUNC <= func)  {
        Serial.println ("wifiFuncKey: valid Functions keys 0-28");
        return 1;
    }

    switch (cmd)  {
    case FUNC_CLR:
        funcState [func] = 0;
        break;

    case FUNC_SET:
        funcState [func] = 1;
        break;

    case FUNC_TGL:
    default:
        funcState [func] ^= 1;
        break;
    }

    sprintf (s, "TF%d%d", funcState [func], func);
    wifiSend (s);
}

// -------------------------------------
// send jmri function commands for various buttons

enum { F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13 };

struct funcBut_s {
    int     func;
    int     but;
    bool    state;
} funcButs [] = {
    { F0,  BUT_LAMP },
    { F1,  BUT_HORN },
    { F5,  BUT_G    },  // testing grp 2
    { F8,  BUT_H    },
    { F9,  BUT_I    },  // testing grp 3
    { F13, BUT_J    },  // testing grp 4
};
#define N_FUNC_BUTS  (sizeof(funcButs)/sizeof(struct funcBut_s))

static void chkFunctions (void)
{
 // FuncBut_t  *f = funcButs;
    struct funcBut_s  *f = funcButs;

    for (int n = 0; n < N_FUNC_BUTS; n++, f++)  {
        if (f->state != butState [f->but])  {
            f->state  = butState [f->but];

            sprintf (s, "TF%d%d", ! f->state, f->func - F0);
            wifiSend (s);
        }
    }
}

// -------------------------------------
// check if loco # has changes
//     update jmri with new # or
//     send dispatch/release to jmri
void chkLoco (void)
{
    static int  locoLst = 0;

    if (locoLst == loco)
        return;

    sprintf (s, "%s: loco %d, locoLst %d,", __func__, loco, locoLst);
    Serial.println (s);

    if (0 != locoLst)  {
        state |= ST_NO_LOCO;
        wifiSend ("MT-*<;>r");    // release all
        wifiSend ("TS0");
    }

    if (0 != loco)  {
        state &= ~ST_NO_LOCO;

        sprintf (s, "T%c%d", 128 < loco ? 'L' : 'S', loco);
        wifiSend (s);
    }

    locoLst = loco;
}

// ---------------------------------------------------------
// display wifi responses on serial monitor
static void wifiReceive (void)
{
    static char cLst = 0;

    while (wifi.available()) {
        char c = wifi.read();
        if ('\r' == c)
            continue;

        if ('\n' == cLst && '\n' == c)
            continue;

            Serial.write (c);
        cLst = c;
    }
}

// ---------------------------------------------------------
// common routine for sending strings to wifi and flushing
void
wifiSend (
    const char*  s )
{
    Serial.print ("wifiSend: ");
    Serial.println (s);

    wifi.println (s);
    wifi.flush ();
}

// -----------------------------------------------------------------------------
// display up to 4 lines of text
void dispOled(
    const char  *s0,
    const char  *s1,
    const char  *s2,
    const char  *s3,
    bool         clr )
{
    char  s [40];

    if (clr)
        display.clear();

    display.setTextAlignment(TEXT_ALIGN_LEFT);

    if (s0)  {
        display.drawString(0, DISP_Y0,  s0);
        if (debug && NUL != *s0)  {
            sprintf (s, "... %s", s0);
            Serial.println (s);
        }
    }
    if (s1)  {
        display.drawString(0, DISP_Y1, s1);
        if (debug && NUL != *s1)  {
            sprintf (s, "    %s", s1);
            Serial.println (s);
        }
    }
    if (s2)  {
        display.drawString(0, DISP_Y2, s2);
        if (debug && NUL != *s2)  {
            sprintf (s, "    %s", s2);
            Serial.println (s);
        }
    }
    if (s3)  {
        display.drawString(0, DISP_Y3, s3);
        if (debug && NUL != *s3)  {
            sprintf (s, "    %s", s3);
            Serial.println (s);
        }
    }
    display.display();
}

// -----------------------------------------------------------------------------
void loop()
{
    static unsigned long msecLst  = 0;
    msec    = millis();
    timeSec = msec / 1000;

    // some debuging
    static int stateLst = 0;
    if (debug && stateLst != state)  {
        stateLst = state;

        printf ("%s: state 0x%02x -", __func__, state);
        for (unsigned i = 0; i < N_STATE_STR; i++)
            if (state & (1<<i))
                printf (" %s", stateStr [i]);
        printf ("\n");
    }

    // -------------------------------------
    // attempt wifi connection
    if (! (ST_WIFI & state) && ! (ST_CFG & state))  {
        if (WL_CONNECTED == WiFi.begin (ssid, pass))  {
            state |= ST_WIFI;

            IPAddress ip = WiFi.localIP ();
            sprintf (s, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

            dispOled("WiFi connected", 0, s, 0, CLR);
            serverInit ();
            delay (3000);
        }
        else  {
            delay (1000);
            dispOled("WiFi connecting", ssid, pass, 0, CLR);
#if 0
            sprintf ((char*)"WiFi connecting, %s, %s", ssid, pass);
            Serial.println (s);
#endif

            delay (1000);
            dispOled(0, ssid, pass, 0, CLR);
        }
    }

    // -------------------------------------
    // attempt jmri connection if WiFi established
    else if (! (ST_JMRI & state) && ! (ST_CFG & state))  {
        sprintf (s0, "%d", port);
        dispOled("JMRI connecting", host, s0, 0, CLR);

#if 0
        sprintf (s, "JMRI connecting, %s, %s", host, s0, 0);
        Serial.println (s);
#endif

        if (wifi.connect(host, port))  {
            state |= ST_JMRI;

            dispOled("JMRI connected", 0, 0, 0, CLR);
            sprintf (s, "N%s", project);
            wifiSend (s);

            delay (2000);
        }

        dispOled(0, host, s0, 0, CLR);
        delay (1000);
    }

    // -------------------------------------
    // display default screen
    else if (! (ST_CFG & state))  {
        char *t = NULL;

        if (! (ST_WIFI & state))
            t = (char*) "No WiFi";
        else if (! (ST_JMRI & state))
            t = (char*) "No JMRI";
        else if (ST_NO_LOCO & state)
            t = (char*) "No LOCO";
        else  {
            sprintf (s, "%2d:%02d   %d", timeSec / 60, timeSec % 60, loco);
            sprintf (s0, "   %3d Thr  %d Brake", throttle, brake);
            sprintf (s1, "   %3d Spd  %s Dir", mph,
                    DIR_NEUTRAL == dir ? "NEU"
                        : DIR_FOR == dir ? "For" : "Rev");
        }

        if (t) {
            sprintf (s, "%2d:%02d   %s", timeSec / 60, timeSec % 60, t);
            strcpy (s0, "");
            strcpy (s1, "");
        }

        static int timeSecLst = 0;
        if (timeSecLst != timeSec)  {
            timeSecLst = timeSec;
            dispOled (s, s0, s1, 0, CLR);
        }
    }

    // -------------------------------------
    // scan interfaces

    // -------------------------------------
    // scan external interfaces
    server ();
    wifiReceive ();

    // check serial I/Fs and update state appropriately
    int res = pcRead (Serial);
#ifdef BT
    if (! res)
        res = pcRead (serialBT);
#endif
    state = res ? state | ST_CFG : state & ~ST_CFG;

    // -------------------------------------
    // update JMRI
    chkLoco ();
    if (! (ST_NO_LOCO & state))
        physics (msec);

    delay (100);        // debounce for buttons
}

// -----------------------------------------------------------------------------
void
setup (void)
{
    Serial.begin (115200);
    Serial.print   (project);
    Serial.print   (" - ");
    Serial.println (version);

#ifdef BT
    serialBT.begin (project);
#endif

    SPIFFS.begin (true);

    if (digitalRead (TF))  {
#if 1
        varsLoad ();
#endif
    }

    // config all inputs with PULLUP
    for (int i=0; i < N_BUTTONS; i++)
        pinMode (pinPullups [i], INPUT_PULLUP);

    // init OLED display
    display.init();
 // display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setColor(WHITE);

    // -------------------------------------
    dispOled(project, version, 0, 0, CLR);

    state = ST_NO_LOCO;

    delay (2000);
}
