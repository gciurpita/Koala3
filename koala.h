#ifndef KOALA_H
# define KOALA_H

# define NUL        0

# define DISP_Y0    0
# define DISP_Y1    14
# define DISP_Y2    28
# define DISP_Y3    42

# define CLR     1

# define SH1106
# ifdef SH1106
#  include "SH1106Wire.h"
extern SH1106Wire  display;
# else
#  include "SSD1306Wire.h"
extern SSD1306Wire  display;
# endif


extern unsigned int debug;
extern void dispOled (const char*, const char*, const char*, const char*, bool); 
// extern char s0[30];
// extern char s1[30];
extern char s [80];

enum { FUNC_CLR, FUNC_SET, FUNC_TGL };
int wifiFuncKey (unsigned func, int cmd);

void wifiSend (const char*);

#endif
