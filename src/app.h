#ifndef APP_H
#define APP_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef AP_3DO
#include "stdio.h" //

#include <displayutils.h>
#include <operamath.h>
#include <io.h>
#include <mem.h>
#include <event.h>
#include <graphics.h>
//#include <filestream.h>
//#include <filestreamfunctions.h>

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

#else
#include <stdio.h>
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

#define DRAM_SIZE   (768 * 1024)
#define VRAM_SIZE   (640 * 1024)

#ifdef AP_3DO
    #define assert(expr)
#else
    #define assert(expr) if (!(expr)) { printf("ASSERT:\n  %s:%d\n  %s => %s", __FILE__, __LINE__, __FUNCTION__, #expr); __debugbreak(); }
#endif

#define qmemcpy memcpy

#define PER_DGT_A   (1 << 0)
#define PER_DGT_B   (1 << 1)
#define PER_DGT_C   (1 << 2)
#define PER_DGT_X   (1 << 3)
#define PER_DGT_Y   (1 << 4)
#define PER_DGT_Z   (1 << 5)
#define PER_DGT_TL  (1 << 6)
#define PER_DGT_TR  (1 << 7)
#define PER_DGT_U   (1 << 8)
#define PER_DGT_D   (1 << 9)
#define PER_DGT_L   (1 << 10)
#define PER_DGT_R   (1 << 11)
#define PER_DGT_S   (1 << 12)

sint32 app_time(void);
uint16 app_input(void);
sint32 app_poll(void);
void app_init(void);

#endif
