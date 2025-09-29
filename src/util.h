#ifndef __INCLUDEDutilh
#define __INCLUDEDutilh

#include <sega_mth.h>
#include <sega_spr.h>
#include "slevel.h"

#ifdef PAL
#define FRAMESPERSEC 50
#else
#define FRAMESPERSEC 60
#endif

#ifndef NDEBUG
extern sint32 extraStuff;
#endif

#define MAXNMSECTORS 600
#define MAXNMWALLS 5500
#define ROUTESIZE 8
#define NMOBJECTPALLETES 5

#define DISABLE INT_ChgMsk(INT_MSK_NULL, INT_MSK_DMA0 | INT_MSK_SPR)
#define ENABLE INT_ChgMsk(INT_MSK_DMA0 | INT_MSK_SPR, INT_MSK_NULL)

#define qmemcpy memcpy

extern uint32 systemMemory;

extern sint8 cheatsEnabled;
extern sint8 enable_stereo;
extern sint8 enable_music;

sint32 bitScanBackwards(uint32 i, sint32 start);
sint32 bitScanForward(uint32 i, sint32 start);

fix32 evalHermite(fix32 t, fix32 p1, fix32 p2, fix32 d1, fix32 d2);
fix32 evalHermiteD(fix32 t, fix32 p1, fix32 p2, fix32 d1, fix32 d2);

sint32 approxDist(sint32 dx, sint32 dy, sint32 dz);
fix32 dist(fix32 dx, fix32 dy, fix32 dz);
sint32 getAngle(sint32 dx, sint32 dy);
sint32 normalizeAngle(sint32 angle);
sint32 fixSqrt(sint32 n, sint32 frac);

MthXyz* getVertex(sint32 vindex, MthXyz* out);
void setGreyTableBalance(sint32 r, sint32 g, sint32 b); /* 0-31 */
extern uint16 greyTable[];

sint32 findCeilDistance(sint32 s, MthXyz* p);
sint32 findFloorDistance(sint32 s, MthXyz* p);

char* catFixed(char* buffer, sint32 fixed, sint32 frac);
void message(char* message);

#ifndef NDEBUG

void assertFail(char* file, sint32 line);

#define assert(x)                           \
    do                                      \
        if (!(x))                           \
            assertFail(__FILE__, __LINE__); \
    while (0)
#define validPtr(x)                                                                                                \
    do                                                                                                             \
        if (!((((sint32)x >= 0x200000 && ((sint32)x) <= 0x2fffff)) || (((sint32)x) >= 0x6004000 && ((sint32)x) <= 0x60fffff))) \
            assertFail(__FILE__, __LINE__);                                                                        \
    while (0)

#else

#define assert(x)
#define validPtr(x)

#endif

#define F(a) ((a) << 16)
#define f(a) ((a) >> 16)

#ifdef TODO
#define MTH_Mul(a, b) fixMul(a, b)
extern fix32 fixMul(fix32 a, fix32 b)
{
    fix32 c;
    __asm__ volatile("dmuls.l %1,%2\n"
                     "sts mach,r11\n"
                     "sts macl,%0\n"
                     "xtrct r11,%0"
                     : "=r"((fix32)c)
                     : "r"((fix32)a), "r"((fix32)b)
                     : "mach", "macl", "r11");
    return c;
}
#endif

extern fix32 getStackPointer(void);

#ifndef NDEBUG
extern void _checkStack(char* file, sint32 line);
#define checkStack() _checkStack(__FILE__, __LINE__)
#else
#define checkStack()
#endif

#if 0
#define MTH_Product(a, b) fixProduct(a, b)
extern fix32 fixProduct(fix32 *a,fix32 *b)
{fix32 c;
 __asm__ ("clrmac\n"
		   "mac.l @%1+,@%2+\n"
		   "mac.l @%1+,@%2+\n"
		   "mac.l @%1+,@%2+\n"
		   "sts mach,r11\n"
		   "sts macl,%0\n"
		   "xtrct r11,%0"
                   : "=r" ((fix32)c), "=r" (a), "=r" (b)
		   : "1" (a), "2" (b)
                   : "mach","macl","r11");
 return c;
}
#endif

void mem_init(void);
void* mem_nocheck_malloc(sint32 area, sint32 size);
void* mem_malloc(sint32 area, sint32 size);
void mem_free(void*);
sint32 mem_coreleft(sint32 area);
void mem_lock(void);

#ifdef PSYQ
void debugPrint(char* message);
#define dPrint(format, args...)        \
    {                                  \
        char buff[80];                 \
        sprintf(buff, format, ##args); \
        debugPrint(buff);              \
    }
#else
#include <stdio.h>
#define debugPrint      printf
#define dPrint          printf
#endif

#ifndef NDEBUG
extern sint32 debugFlag;
extern sint32 extraStuff;
#endif

#define RGB(r, g, b) (0x8000 | ((b) << 10) | ((g) << 5) | (r))
#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))

void resetEnable(void);
void resetDisable(void);

#define POKE_B(adr, data) (*((uint8*)(adr)) = ((uint8)(data)))
#define POKE_W(adr, data) (*((uint16*)(adr)) = ((uint16)(data)))
#define POKE(adr, data) (*((uint32*)(adr)) = ((uint32)(data)))
#define PEEK(adr) (*((uint32*)(adr)))
#define PEEK_W(adr) (*((uint16*)(adr)))
#define PEEK_B(adr) (*((uint8*)(adr)))

/* from the GCC FAQ */

/*
** void Set_Hardware_Divide(sint32, sint32);
**
** Set the dividend and divisor of the hardware divide unit.
** The divider requires 37 clocks to calculate a result,
** so you want to execute some other code before retrieving the result.
*/
#if 0
#define Set_Hardware_Divide(x, y)                                       \
    ({                                                                  \
        sint32* div_unit = (sint32*)0xffffff00;                               \
        sint32 dividend = x, divisor = y;                                  \
        __asm__ volatile("mov.l %1,@%2; mov.l %0,@(4,%2);"              \
                         : /* no output */                              \
                         : "r"(dividend), "r"(divisor), "r"(div_unit)); \
    });

#define Set_Hardware_DivideFixed(x, y)                                                          \
    ({                                                                                          \
        sint32* div_unit = (sint32*)0xffffff00;                                                       \
        sint32 dividend = x, divisor = y;                                                          \
        __asm__ volatile("mov.l %1,@%2; mov.l %0,@(0x10,%2); mov.l #0,r1; mov.l r1,@(0x14,%2);" \
                         : /* no output */                                                      \
                         : "r"(dividend), "r"(divisor), "r"(div_unit)                           \
                         : "r1");                                                               \
    });

/*
** sint32 Get_Hardware_Divide(void)
**
** Retrieves division result from the hardware divide unit.
** If less than 37 clocks have elapsed the CPU will be halted
** until the result is ready.
*/

#define Get_Hardware_Divide()                                                     \
    ({                                                                            \
        sint32* div_unit = (sint32*)0xffffff00;                                         \
        sint32 __result;                                                             \
        __asm__ volatile("mov.l @(0x1c,%1),%0" : "=r"(__result) : "r"(div_unit)); \
        __result;                                                                 \
    });
#else
extern fix32 div_ret;

#define Set_Hardware_Divide(x, y) div_ret = (x) / (y)
#define Get_Hardware_Divide() div_ret
#define Set_Hardware_DivideFixed(x, y) div_ret = MTH_Div(x, y)

#endif

extern uint16 buttonMasks[];
enum
{
    ACTION_FIRE,
    ACTION_JUMP,
    ACTION_PUSH,
    ACTION_FREELOC,
    ACTION_WEPDN,
    ACTION_WEPUP,
    ACTION_STRAFE,
    ACTION_RUN
};
extern sint8 controllerConfig[8];
#define IMASK(action) (buttonMasks[(sint32)controllerConfig[(sint32)(action)]])

uint16 getNextRand(void);

#define BREAK asm("trapa #34")

void getDateTime(sint32* year, sint32* month, sint32* day, sint32* hour, sint32* min);

sint32 findWallsSector(sint32 wallNm);

void displayEnable(sint32 state);

sint32 findSectorHeight(sint32 s);

void delay(sint32 frames);

#endif
