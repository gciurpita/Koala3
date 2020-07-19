// web server allowing browser to change parameters
// Rui Santos -- http://randomnerdtutorials.com

#include <Arduino.h>
#include <WiFi.h>

#include "koala.h"
#include "server.h"
#include "vars.h"

// Set web server port number to 80
WiFiServer _server(80);

#define RESP_SIZE 3000
char resp [RESP_SIZE];

// Auxiliar variables to store the current output state
// Assign output variables to GPIO pins
const int pinA = 19;
const int pinB = 18;

bool pinAState = 0;
bool pinBState = 0;

// -----------------------------------------------------------------------------
void
serverInit ()
{
    _server.begin();

    dispOled ("Server UP", 0, 0, 0, CLR);

    pinMode (pinA, OUTPUT);
    pinMode (pinB, OUTPUT);

    digitalWrite (pinA, HIGH);
    digitalWrite (pinB, HIGH);
}

// -----------------------------------------------------------------------------
// send html header and page string
void
sendResp (
    WiFiClient & client,
    char *resp )
{
    char s [90] = {};

    strcat (s, "HTTP/1.1 200 OK\r\n");
    strcat (s, "Content-Type: text/html\r\n");
    client.print (s);

    bzero (s, 90);
    sprintf (s, "Content-Length: %d\r\n\r\n", strlen (resp));
    client.print (s);

    Serial.print (__func__);
    Serial.print (": ");
    Serial.println (s);

    client.print (resp);
}

// ---------------------------------------------------------
// format a form entry
void
labelInput (
    WiFiClient & client,
    char        *resp,
    const char  *name,
    char        *val )
{
    char  s [200] = {};
    sprintf (s, 
                " <label for=\"%s\">%s</label><br>\r\n"
                " <input type=\"text\" id=\"%s\" name=\"%s\" value=\"%s\">"
                " <br><br>\r\n",
                name, name, val, name, name);

    strcat (resp, s);
}

// ---------------------------------------------------------
// format a form entry
void
formEntry (
    WiFiClient & client,
    char        *resp,
    const char  *name,
    char        *val )
{
    char  s [200] = {};
    sprintf (s, " <input type=\"text\" id=\"%s\" name=\"%s\" value=\"%s\">\r\n"
                "  <label for=\"%s\">%s</label><br>\r\n",
                name, name, val, name, name);

    strcat (resp, s);
}

// ---------------------------------------------------------
// create the paramter page using an html form
void
sendForm (
    WiFiClient & client)
{
    if (debug)
        printf ("  %s:\n", __func__);

    bzero (resp, RESP_SIZE);

    strcat (resp, "<h2> Koala Throttle <h2>\r\n");
    strcat (resp, "<form action=\"/go\">\r\n");

    // create form entries from vars table entries

    for (EeVar_t *e = pEeVars; NULL != e->p; e++)  {
        char s [MAX_CHAR];
        if (V_INT == e->type)  {
            sprintf (s, "%d", *(int*)e->p);
        }
        else
            strcpy (s, (char*)e->p);
#if 0
        labelInput (client, resp, e->desc, s);
#else
        formEntry (client, resp, e->desc, s);
#endif
    }

    strcat (resp, " <input type=\"submit\" value=\"Submit\">\r\n");
    strcat (resp, "</form>\r\n");

    sendResp (client, resp);
}
 
// -----------------------------------------------------------------------------
const char *webPage =  {
    "<!DOCTYPE html><html>"
    "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<link rel=\"icon\" href=\"data:,\">"
    "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; }"
    ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; "
    "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; }"

    ".button2 {background-color: #555555; }</style></head>"
    "<body><h1>ESP32 Web Server</h1>"
    "<p>GPIO 26 - State </p>"
    "<p><a href=\"/26/tgl\"><button class=\"button\">TGL</button></a></p>"

    "<p>GPIO 27 - State </p>"
    "<p><a href=\"/27/tgl\"><button class=\"button\">TGL</button></a></p>"
    "</body></html>"
};

// -----------------------------------------------------------------------------
void
server (void)
{
 // static char header [512] = {};
 // static int  idx          = 0;

    WiFiClient client = _server.available();

    // Listen for incoming clients
    if (! client)
        return;

    Serial.println("New Client.");
    // print a message out in the serial port
    String currentLine = "";

    // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
        if (! client.available())
            continue;
        
        // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor

 //     header [idx++] = c;

        if (c != '\n')  {
            if (c != '\r')  // append anything but carriage return
                currentLine += c;
            continue;
        }

        // if the byte is a newline character
        // if the current line is blank, rec two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0) {
            // headers start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming,
            // then a blank line:

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

#if 0
            // turns the GPIOs on and off
            if (strstr (header, "GET /26/tgl") != 0)  {
                pinAState = ! digitalRead (pinA);
                digitalWrite (pinA, pinAState);

                sprintf (s, "GPIO %d tgl - %d", pinA, pinAState);
                Serial.println(s);
            }

            else if (strstr (header, "GET /27/tgl") != 0)  {
                pinBState = ! digitalRead (pinB);
                digitalWrite (pinB, pinBState);

                sprintf (s, "GPIO %d tgl - %d", pinB, pinBState);
                Serial.println(s);
            }

            client.println(webPage);
#else
            sendForm (client);
#endif
            break;
        }
        else { // if you got a newline, then clear currentLine
            currentLine = "";
        }
    }

    // Clear the header variable
 // idx = 0;

    // Close the connection
 // client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
}
