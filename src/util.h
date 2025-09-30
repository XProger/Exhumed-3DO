#ifndef __INCLUDEDutilh
#define __INCLUDEDutilh

#include "mth.h"
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

#ifndef NDEBUG

#define validPtr(x)

#else

#define validPtr(x)

#endif

#define F(a) ((a) << 16)
#define f(a) ((a) >> 16)

void mem_init(void);
void* mem_nocheck_malloc(sint32 area, sint32 size);
void* mem_malloc(sint32 area, sint32 size);
void mem_free(void*);
sint32 mem_coreleft(sint32 area);
void mem_lock(void);

#define debugPrint      printf
#define dPrint          printf

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
