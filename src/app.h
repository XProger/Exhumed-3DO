#ifndef APP_H
#define APP_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef AP_3DO
#include "stdio.h" // I'm not passionate enough to find out why 3do-devkit require " to make printf work
#else
#include <stdio.h>

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

#ifdef AP_3DO
    #define FS_INT(p)   *(p)
    #define FS_SHORT(p) *(p)
#else
    #define FS_INT(p)   (sint32)( ((uint8*)(p))[3] | (((uint8*)(p))[2] << 8) | (((uint8*)(p))[1] << 16) | (((uint8*)(p))[0] << 24) )
    #define FS_SHORT(p) (sint32)( ((uint8*)(p))[1] | (((uint8*)(p))[0] << 8) )
#endif

//#define DRAM_SIZE   (256 * 1024)
#define DRAM_SIZE   (256 * 1024)
#define VRAM_SIZE   (704 * 1024)

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

void fs_open(const char *name);
void fs_read(void *buffer, sint32 count);
void fs_skip(sint32 offset);
void fs_close(void);

#define fs_progress_start(swirly)
#define fs_progress_add(filename)
#define fs_progress_stop()

#define track_play(track, repeat)
#define track_stop()

#endif
