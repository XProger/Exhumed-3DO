#ifndef APP_H
#define APP_H

#ifdef AP_3DO
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <displayutils.h>
#include <operamath.h>
#include <io.h>
#include <mem.h>
#include <event.h>
#include <graphics.h>
//#include <filestream.h>
//#include <filestreamfunctions.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
#endif

typedef sint32 fix32;

#ifndef NULL
    #define NULL ((void*)0)
#endif

#define FIXED(a) ((fix32)((a) * 65536.0))
#define DIV_FIXED(a, b) MTH_Div(a, b)

#define FS_INT(p)   (sint32)( ((uint8*)(p))[3] | (((uint8*)(p))[2] << 8) | (((uint8*)(p))[1] << 16) | (((uint8*)(p))[0] << 24) )
#define FS_SHORT(p) (sint32)( ((uint8*)(p))[1] | (((uint8*)(p))[0] << 8) )

sint32 app_time(void);
uint16 app_input(void);
sint32 app_poll(void);
void app_init(void);

#endif
