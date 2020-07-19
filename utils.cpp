
#include <Arduino.h>

#define Width   128

extern const uint8_t ArialMT_Plain_10[] PROGMEM;
extern const uint8_t ArialMT_Plain_16[] PROGMEM;
extern const uint8_t ArialMT_Plain_24[] PROGMEM;

const uint8_t *font = ArialMT_Plain_16;

// --------------------------------------------------------------------
// Font Table
//     4 header bytes:   width, height, 1st font character, # chars
//     (224) char table: msb & lsb of font offset, # bytes, font width
//     font data: column data, # of bytes >= ht / 8

#include <OLEDDisplayFonts.h>

// offsets
#define WIDTH_POS       0
#define HEIGHT_POS      1
#define FIRST_CHAR_POS  2
#define CHAR_NUM_POS    3

#define JUMPTABLE_START 4
#define JUMPTABLE_BYTES 4

#define JUMPTABLE_MSB   0
#define JUMPTABLE_LSB   1
#define JUMPTABLE_SIZE  2
#define JUMPTABLE_WIDTH 3

// --------------------------------------------------------------------
byte fontWid = font [WIDTH_POS];
byte fontHt  = font [HEIGHT_POS];
byte firstCh = font [FIRST_CHAR_POS];
byte numChar = font [CHAR_NUM_POS];

int  jumpTblSize = numChar * JUMPTABLE_BYTES;


// ------------------------------------------------
int
fontStringWid (
    const char  *str)
{
    int  wid = 0;
    int  firstChar  =  pgm_read_byte(font + FIRST_CHAR_POS);

    for (int i = 0; i < strlen(str); i++)  {
        int  chWid = pgm_read_byte (font
                        + JUMPTABLE_START
                        + (str [i] - firstChar) * JUMPTABLE_BYTES
                        + JUMPTABLE_WIDTH );
        wid +=  chWid;
    }

    return wid;
}
