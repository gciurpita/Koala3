// serial command processor
// mode: cfg, cmd ad pins

#include <Arduino.h>

#include "file.h"
#include "koala.h"
#include "pcRead.h"
#include "vars.h"

enum { PinMode, CmdMode, CfgMode };
static int _mode = PinMode;

// -----------------------------------------------------------------------------
static void
_dispEeVars (
    Stream &Serial)
{
    EeVar_t  *e = pEeVars;
    int       loc = 0;

    sprintf (s, "%s:", __func__);
    Serial.println (s);

    for (int i = 0; NULL != e->p; i++, loc += e->nByte, e++) {
        sprintf (s, "    %2d %4d:", e->nByte, loc);
        Serial.print (s);

        if (V_STR == e->type)
            sprintf (s, " %10s - %s", e->desc, (char*)e->p);
        else
            sprintf (s, " %10s - %d", e->desc, *(int *)e->p);
        Serial.println (s);
    }
}

// -----------------------------------------------------------------------------
static void
_dispVars (
    Stream &Serial)
{
    Vars_s   *v = pVars;

    sprintf (s, "%s:", __func__);
    Serial.println (s);

    for (int i = 0; 0 != v->p; i++, v++)  {
        sprintf (s, "    %6d  %s\n", *(v->p), v->desc);
        Serial.print (s);
    }
}

// ---------------------------------------------------------
// edit throttle parameters thru serial monitor
#define MAX_BUF 40
int  _cfgHdr = 0;

static int
_cfgMode (
    Stream &Serial)
{
    static int  cnt    = 0;
    static int  stRead = 0;
    static int  idx    = 0;
    static char buf [MAX_BUF] = {};

    static EeVar_t   *e  = pEeVars;

    if (! _cfgHdr++) {
        sprintf (s, "%s: enter 'q' to exit", __func__);
        Serial.println (s);
    }

    if (stRead)  {
        if (Serial.available()) {
            char c   = Serial.read ();

            if (0 == idx)  {    // check for char on first column
                switch (c)  {
                case 'q':
                    Serial.println ("pcRead: switch to Cmd mode");
                    _mode = CmdMode;             // what about ssid/password
                    return 1;

                case 'S':
                    varsSave ();
                    return 0;

                default:
                    break;
                }
            }

        //  if ('\n' == c && ! cnt++)  {    // ignore very first \n
            if ('\n' == c)  {
                if (0 < idx)  {
                    buf [idx] = 0;

                    if (V_STR == e->type)
                        strcpy ((char*) e->p, buf);
                    else
                        *(int*)e->p = atoi (buf);
                }
                else  {
                    e++;
                    e = NULL == e->p ? pEeVars : e;
                }
                stRead = idx = 0;
                return 1;
            }

            buf [idx++] = (char) c;
        }

    }
    else  {
        if (V_STR == e->type)
            sprintf (s, " > %10s: %s", e->desc, (char*)e->p);
        else
            sprintf (s, " > %10s: %d", e->desc, *(int*) e->p);
        Serial.println (s);
        stRead ++;
    }

    return 1;
}

// -----------------------------------------------------------------------------
static void
_cmdModeHelp (
    Stream &Serial)
{
    Serial.println ("\ncmdMode:");
    Serial.println ("   # B - set button #");
    Serial.println ("   # c - set cars to #");
    Serial.println ("   # b - set train brake to #");
    Serial.println ("     d - list SPIFFS files");
    Serial.println ("     D - set debug to #");
    Serial.println ("     E - display configuration variables");
    Serial.println ("     e - switch to cfgMode");
    Serial.println ("     f - toggle decoder function #");
    Serial.println ("   # I - set independent brake to #");
    Serial.println ("     L - load configuration");
    Serial.println ("   # l - set loco address to #");
    Serial.println ("   # r - set reverser #");
    Serial.println ("     S - save configuration");
    Serial.println ("   # t - set throttle #");
    Serial.println ("     m - switch to pinMode");
    Serial.println ("     v - display state variables");
    Serial.println ("     V - print version");
}

// -----------------------------------------------------------------------------
// process single character commands from the PC
static int
_cmdMode (
    Stream &Serial )
{
    static int  val  = 0;
    char buf [40] = {};
    int  idx      = 0;

    while (Serial.available())
        buf [idx++] = Serial.read ();

    if (0 == idx)
        return 0;

    for (int i = 0; i < idx; i++)  {
        char c = buf [i];

        switch (c)  {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            val = c - '0' + (10 * val);
            continue;     // only time val needs to be preserved

        case 'b':   // independent brake
            brake = MAX_BRK < val ? MAX_BRK : val;
            break;
    
        case 'c':
            cars = val;
            break;
    
        case 'B':
            // reserved for buttons
            break;
    
        case 'D':
            debug = val;
            break;
    
        case 'd':
            fileDir ();
            break;
    
        case 'E':
            _dispEeVars (Serial);
            break;
    
        case 'e':
            _cfgHdr = 0;
            _mode = CfgMode;
            break;
    
        case 'f':
            wifiFuncKey ((unsigned int) val, FUNC_TGL);
            break;
    
        case 'I':   // independent brake
            brakeInd = MAX_BRK < val ? MAX_BRK : val;
            break;
    
        case 'L':
            varsLoad ();
            break;

        case 'l':
            loco = val;
            printf ("%s: %d %d loco\n", __func__, loco, val);
            throttle = 0;
            reverser = MAX_REV / 2;
            val = 0;
            break;
    
        case 'r':
            reverser = MAX_REV < val ? MAX_REV : val;
            val = 0;
            break;
    
        case 'S':
            varsSave ();
            break;

        case 't':
            throttle = val;
            throttle = MAX_THR < throttle ? MAX_THR : throttle;
            val = 0;
            break;
    
        case 'm':
            _mode = PinMode;
            break;
    
        case 'V':
            Serial.print ("\nversion: ");
            Serial.println (version);
            break;
    
        case 'v':
            _dispVars (Serial);
            break;
    
        case '\n':      // ignore
        case '\r':      // ignore
            if (1 == idx)
                return 0;
            break;
    
        case '?':
            _cmdModeHelp (Serial);
            break;
    
        default:
            Serial.print ("unknown char ");
            Serial.println (c,HEX);
            break;
        }

        val = 0;
    }

    return 1;
}


// -----------------------------------------------------------------------------
static void
_pinModeHelp (
    Stream &Serial)
{
    Serial.println ("\npinMode:");
    Serial.println ("   # A - set analog pin #");
    Serial.println ("   # d - set debug to #");
    Serial.println ("   # I - set pin # to INPUT");
    Serial.println ("   # O - set pin # to OUTPUT");
    Serial.println ("   # P - set pin # to INPUT_PULLUP");
    Serial.println ("   # a - analogRead (pin #)");
    Serial.println ("   # c - digitalWrite (pin #, LOW)");
    Serial.println ("     m - switch to cmd _mode");
    Serial.println ("   # p - analogWrite (analogPin, #)");
    Serial.println ("   # r - digitalRead (pin #)");
    Serial.println ("   # s - digitalWrite (pin #, HIGH)");
    Serial.println ("   # t - toggle pin # output");
    Serial.println ("     v - print version");
}

// -----------------------------------------------------------------------------
// process single character commands from the PC
static int
_pinMode (
    Stream &Serial )
{
    static int  analogPin = 0;
    static int  val  = 0;

    if (Serial.available()) {
        char c   = Serial.read ();

        switch (c)  {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            val = c - '0' + (10 * val);
            return 1;     // only time val needs to be preserved
    
        case 'A':
            analogPin = val;
            Serial.print   ("analogPin = ");
            Serial.println (val);
            val = 0;
            break;
    
        case 'a':
            Serial.print   ("analogRead: ");
            Serial.println (analogRead (val));
            val = 0;
            break;
    
        case 'c':
            digitalWrite (val, LOW);
            Serial.print   ("digitalWrite: LOW  ");
            Serial.println (val);
            val = 0;
            break;
    
        case 'd':
            debug = val;
            val   = 0;
            break;
    
        case 'I':
            pinMode (val, INPUT);
            Serial.print   ("pinMode ");
            Serial.print   (val);
            Serial.println (" INPUT");
            val = 0;
            break;
    
        case 'm':
            _mode = CmdMode;
            break;
    
        case 'O':
            pinMode (val, OUTPUT);
            Serial.print   ("pinMode ");
            Serial.print   (val);
            Serial.println (" OUTPUT");
            val = 0;
            break;
    
        case 'P':
            pinMode (val, INPUT_PULLUP);
            Serial.print   ("pinMode ");
            Serial.print   (val);
            Serial.println (" INPUT_PULLUP");
            val = 0;
            break;
    
        case 'p':
         // analogWrite (analogPin, val);
            Serial.print   ("analogWrite: pin ");
            Serial.print   (analogPin);
            Serial.print   (", ");
            Serial.println (val);
            val = 0;
            break;
    
        case 'r':
            Serial.print   ("digitalRead: pin ");
            Serial.print   (val);
            Serial.print   (", ");
            Serial.println (digitalRead (val));
            val = 0;
            break;
    
        case 'S':
            varsSave ();
            break;
    
        case 's':
            digitalWrite (val, HIGH);
            Serial.print   ("digitalWrite: HIGH ");
            Serial.println (val);
            val = 0;
            break;
    
        case 't':
            digitalWrite (val, ! digitalRead (val));
            val = 0;
            break;
    
        case 'v':
            Serial.print ("\nversion: ");
            Serial.println (version);
            break;
    
        case '\n':      // ignore
            break;
    
        case '?':
            _pinModeHelp (Serial);
            break;
    
        default:
            Serial.print ("unknown char ");
            Serial.println (c,HEX);
            break;
        }

        val = 0;
        return 1;
    }

    return 0;
}

// -----------------------------------------------------------------------------
// process single character commands from the PC
int
pcRead (
    Stream &Serial)
{
    static int  analogPin = 0;
    static int  val  = 0;
    static int  edit = 0;

    switch (_mode)  {
    case CfgMode:
        return _cfgMode (Serial);

    case CmdMode:
        return _cmdMode (Serial);

    default:
    case PinMode:
        return _cmdMode (Serial);
    }

    return 0;
}
