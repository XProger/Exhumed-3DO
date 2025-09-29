#include <libsn.h>
#include <sega_mem.h>
#include "sega_spr.h"
#include "sega_scl.h"
#include "sega_mth.h"
#include "print.h"
#include "sega_per.h"
#include "util.h"
#include "spr.h"

#include "level.h"
#include "v_blank.h"

#ifndef NDEBUG
sint32 extraStuff;
#endif

sint8 enable_stereo;
sint8 enable_music;
sint8 cheatsEnabled;

uint32 systemMemory;

#define STICKSIZE 5096
sint32 mystack[STICKSIZE];
void* _stackinit = &mystack[STICKSIZE];

fix32 div_ret;

MthXyz* getVertex(sint32 vindex, MthXyz* out)
{
    out->x = F(level_vertex[vindex].x);
    out->y = F(level_vertex[vindex].y);
    out->z = F(level_vertex[vindex].z);
    return out;
}

sint32 findFloorDistance(sint32 s, MthXyz* p)
{
    sint32 w;
    sWallType* floor;
    for (w = level_sector[s].firstWall; w <= level_sector[s].lastWall; w++)
    {
        if (level_wall[w].normal[1] <= 0)
            continue;
        break;
    }

    if (w > level_sector[s].lastWall)
        return 0;
    assert(w <= level_sector[s].lastWall);

    floor = level_wall + w;
    if (floor->normal[1] == F(1))
    {
        assert(floor->normal[0] == 0 && floor->normal[2] == 0);
        return p->y - F(level_vertex[floor->v[0]].y);
    }
    {
        fix32 planeDist;
        MthXyz wallP;
        getVertex(floor->v[0], &wallP);
        planeDist = (f(p->x - wallP.x)) * floor->normal[0] + (f(p->z - wallP.z)) * floor->normal[2];
        return p->y - (wallP.y - MTH_Div(planeDist, floor->normal[1]));
    }
}

sint32 findCeilDistance(sint32 s, MthXyz* p)
{
    sint32 w;
    sWallType* floor;
    for (w = level_sector[s].firstWall; w <= level_sector[s].lastWall; w++)
    {
        if (level_wall[w].normal[1] >= 0)
            continue;
        break;
    }
    assert(w <= level_sector[s].lastWall);
    floor = level_wall + w;
    if (floor->normal[1] == F(-1))
    {
        assert(floor->normal[0] == 0 && floor->normal[2] == 0);
        return p->y - F(level_vertex[floor->v[0]].y);
    }
    {
        fix32 planeDist;
        MthXyz wallP;
        getVertex(floor->v[0], &wallP);
        planeDist = (f(p->x - wallP.x)) * floor->normal[0] + (f(p->z - wallP.z)) * floor->normal[2];
        return p->y - (wallP.y - MTH_Div(planeDist, floor->normal[1]));
    }
}

#define SQRTTABLESIZE 1024
#define SQRTTABLEBITS 10 /* this must be even */
#define SQRTTABLEMASK 0x3ff
#include "sqrttab.h"

sint32 getAngle(sint32 dx, sint32 dy)
{
    while (dx > F(1) || dx < F(-1))
    {
        dx >>= 1;
        dy >>= 1;
    }
    while (dy > F(1) || dy < F(-1))
    {
        dx >>= 1;
        dy >>= 1;
    }
    return MTH_Atan(dy, dx);
}

uint16 greyTable[33] = { 0x8000, 0x8000 | (1 << 10) | (1 << 5) | 1, 0x8000 | (2 << 10) | (2 << 5) | 2, 0x8000 | (3 << 10) | (3 << 5) | 3, 0x8000 | (4 << 10) | (4 << 5) | 4, 0x8000 | (5 << 10) | (5 << 5) | 5, 0x8000 | (6 << 10) | (6 << 5) | 6, 0x8000 | (7 << 10) | (7 << 5) | 7, 0x8000 | (8 << 10) | (8 << 5) | 8, 0x8000 | (9 << 10) | (9 << 5) | 9, 0x8000 | (10 << 10) | (10 << 5) | 10, 0x8000 | (11 << 10) | (11 << 5) | 11, 0x8000 | (12 << 10) | (12 << 5) | 12, 0x8000 | (13 << 10) | (13 << 5) | 13, 0x8000 | (14 << 10) | (14 << 5) | 14, 0x8000 | (15 << 10) | (15 << 5) | 15, 0x8000 | (16 << 10) | (16 << 5) | 16, 0x8000 | (17 << 10) | (17 << 5) | 17, 0x8000 | (18 << 10) | (18 << 5) | 18, 0x8000 | (19 << 10) | (19 << 5) | 19, 0x8000 | (20 << 10) | (20 << 5) | 20, 0x8000 | (21 << 10) | (21 << 5) | 21, 0x8000 | (22 << 10) | (22 << 5) | 22, 0x8000 | (23 << 10) | (23 << 5) | 23, 0x8000 | (24 << 10) | (24 << 5) | 24, 0x8000 | (25 << 10) | (25 << 5) | 25, 0x8000 | (26 << 10) | (26 << 5) | 26,
    0x8000 | (27 << 10) | (27 << 5) | 27, 0x8000 | (28 << 10) | (28 << 5) | 28, 0x8000 | (29 << 10) | (29 << 5) | 29, 0x8000 | (30 << 10) | (30 << 5) | 30, 0x8000 | (31 << 10) | (31 << 5) | 31, 0x8000 | (31 << 10) | (31 << 5) | 31 };

#if 0
void setGreyTableBalance(sint32 r,sint32 g,sint32 b) /* 0-31 */
(sint32 i;
 for (i=0;i<32;i++)
    {greyTable[i]=
	0x8000|
	(((b*i)/32)<<10)|
	(((g*i)/32)<<5)|
	(((r*i)/32));
    }
}
#endif

fix32 dist(fix32 dx, fix32 dy, fix32 dz)
{
    sint32 d;
    d = f(dx) * f(dx) + f(dy) * f(dy) + f(dz) * f(dz);
    return fixSqrt(d, 0);
}

sint32 approxDist(sint32 dx, sint32 dy, sint32 dz)
{
    sint32 min;
    dx = abs(dx);
    dy = abs(dy);
    dz = abs(dz);
    if (dx < dy)
        min = dx;
    else
        min = dy;
    if (dz < min)
        min = dz;
    return dx + dy + dz - (min >> 1);
}

#define NMPARTS 5

void assertFail(char* file, sint32 line)
{
    uint16 i, sw;
    MthXyz pos[NMPARTS], vel[NMPARTS];
    char* text[NMPARTS] = { NULL, NULL, "Write", "This", "Down" };

    text[1] = file;
    displayEnable(1);
    /** BEGIN ***************************************************************/

    EZ_initSprSystem(1000, 8, 1000, 240, RGB(0, 0, 0));
    initFonts(0, 7);
    SCL_SetFrameInterval(1);
    SPR_SetEraseData(0x8000, 0, 0, 319, 239);
    for (i = 0; i < NMPARTS; i++)
    {
        pos[i].x = (MTH_GetRand() & 0x000fffff) + (140 << 16);
        pos[i].y = (MTH_GetRand() & 0x000fffff) + (100 << 16);
        vel[i].x = (MTH_GetRand() & 0x0007ffff) - 0x3ffff;
        vel[i].y = (MTH_GetRand() & 0x0007ffff) - 0x3ffff;
    }

    i = 0;
    sw = 0;
    for (;;)
    {
        SCL_SetColOffset(SCL_OFFSET_A, SCL_SP0 | SCL_NBG0, 0, 0, 0);

        EZ_openCommand();

        EZ_sysClip();
        EZ_localCoord(0, 0);

        for (i = 0; i < NMPARTS; i++)
        {
            if (pos[i].x < 0 && vel[i].x < 0)
                vel[i].x = -vel[i].x;
            if (pos[i].y < 0 && vel[i].y < 0)
                vel[i].y = -vel[i].y;
            if (pos[i].x > (300 << 16) && vel[i].x > 0)
                vel[i].x = (vel[i].x >> 3) - vel[i].x;
            if (pos[i].y > (200 << 16) && vel[i].y > 0)
                vel[i].y = (vel[i].y >> 3) - vel[i].y;
            pos[i].x += vel[i].x;
            pos[i].y += vel[i].y;
            vel[i].y += 0x1000;
#ifdef TODO // assert
            if (i)
                drawString(pos[i].x >> 16, pos[i].y >> 16, 1, text[i]);
            else
                drawStringf(pos[i].x >> 16, pos[i].y >> 16, 1, "%d", line);
#endif
        }

        EZ_closeCommand();
        SCL_DisplayFrame();

        app_poll();
    }
}

void message(char* message)
{
    uint16 i;
    sint32 data;
    static sint32 sw = 0;
    /** BEGIN ***************************************************************/

    SCL_SetFrameInterval(1);
    SPR_SetEraseData(0x8000, 0, 0, 319, 239);

    i = 0;
    for (;;)
    {
        EZ_openCommand();

        EZ_sysClip();
        EZ_localCoord(0, 0);

        drawString(10, 100, 0, message);

        EZ_closeCommand();
        SCL_DisplayFrame();

        app_poll();

        data = lastInputSample;
        if ((sw && !(data & PER_DGT_A)) || (!sw && !(data & PER_DGT_B)))
        {
            sw = !sw;
            return;
        }
    }
}

char* catFixed(char* buffer, sint32 n, sint32 frac)
{
    char buff[80];
    sint32 bit, div, i;
    sint32 accum;
    if (n < 0)
    {
        strcat(buffer, "-");
        n = -n;
    }
    sprintf(buff, "%d", (int)(n >> frac));
    strcat(buffer, buff);
    accum = 0;
    for (bit = frac - 1, div = 2; bit >= 0; bit--, div += div)
    {
        if ((n >> bit) & 1)
            accum += 1000000000 / div;
    }
    strcat(buffer, ".");
    sprintf(buff, "%d", (int)accum);
    /* print leading zeros */
    for (i = 9 - strlen(buff); i > 0; i--)
        strcat(buffer, "0");
    /* print rest of decimal part */
    strcat(buffer, buff);
    return buffer;
}

/* frac must be even! */
sint32 fixSqrt(sint32 n, sint32 frac)
{
    sint32 shiftCount = 0;
    sint32 e;
    sint32 result;
    assert(!(frac & 1));
    assert(!(n & 0x80000000));
    while (n & (0xffffffff - SQRTTABLEMASK))
    {
        n = n >> 2;
        shiftCount++;
    }
    e = 2 * shiftCount + SQRTTABLEBITS - frac - 2;
    result = sqrtTable[n] >> (31 - frac - e / 2);
    return result;
}

#ifdef TODO
fix32 fixMul(fix32 a, fix32 b)
{
    fix32 c;
    __asm__ volatile("dmuls.l %1,%2\n sts mach,r11\n sts macl,%0\n xtrct r11,%0" : "=r"((fix32)c) : "r"((fix32)a), "r"((fix32)b) : "mach", "macl", "r11");
    return c;
}
#else
#define fixMul(a, b) (((a) * (b)) >> 16)
#endif

fix32 evalHermite(fix32 t, fix32 p1, fix32 p2, fix32 d1, fix32 d2)
{
    fix32 t2 = MTH_Mul(t, t);
    fix32 t3 = MTH_Mul(t2, t);

    return (MTH_Mul(2 * t3 - 3 * t2 + F(1), p1) + MTH_Mul(-2 * t3 + 3 * t2, p2) + MTH_Mul(t3 - 2 * t2 + t, d1) + MTH_Mul(t3 - t2, d2));
}

fix32 evalHermiteD(fix32 t, fix32 p1, fix32 p2, fix32 d1, fix32 d2)
{
    fix32 t2 = MTH_Mul(t, t);

    return (MTH_Mul(6 * t2 - 6 * t, p1) + MTH_Mul(-6 * t2 + 6 * t, p2) + MTH_Mul(3 * t2 - 4 * t + F(1), d1) + MTH_Mul(3 * t2 - 2 * t, d2));
}

#define NMAREAS 2
#define STACKSIZE 8 /* must be power of 2 */
static uint8 *memStack[NMAREAS][STACKSIZE];
static sint32 stackPos[NMAREAS];
static uint8 *areaEnd[NMAREAS];

#define DRAM_SIZE   (256 * 1024)

static uint8 DRAM[2][DRAM_SIZE];

static uint8 *mem1Start = DRAM[0];
static uint8 *mem2Start = DRAM[1];

void mem_init(void)
{
    memStack[0][0] = mem1Start;
    memStack[1][0] = mem2Start;
    stackPos[0] = 0;
    stackPos[1] = 0;
    areaEnd[0] = mem1Start + DRAM_SIZE;
    areaEnd[1] = mem2Start + DRAM_SIZE;
}

void mem_lock(void)
{
    mem1Start = memStack[0][stackPos[0]];
    mem2Start = memStack[1][stackPos[1]];
}

sint32 mem_coreleft(sint32 a1)
{
    return areaEnd[a1] - memStack[a1][stackPos[a1]];
}

void* mem_nocheck_malloc(sint32 area, sint32 size)
{
    sint32 a1 = area;
    uint8 *retAddr;
    assert(area >= 0);
    assert(area < NMAREAS);
    size = (size + 3) & (~3);
    do
    { /* see if we can allocate in this area */
        assert(memStack[a1][stackPos[a1]] < areaEnd[a1]);
        if (areaEnd[a1] - memStack[a1][stackPos[a1]] > size)
        { /* can allocate here */
            retAddr = memStack[a1][stackPos[a1]];
            stackPos[a1] = (stackPos[a1] + 1) & (STACKSIZE - 1);
            memStack[a1][stackPos[a1]] = retAddr + size;
            return (void*)retAddr;
        }
        a1++;
        if (a1 >= NMAREAS)
            a1 = 0;
    } while (a1 != area);
    return NULL;
}

void* mem_malloc(sint32 area, sint32 size)
{
    void* r = mem_nocheck_malloc(area, size);
    assert(r);
    return r;
}

void mem_free(void* p)
{
    sint32 a, s;
    for (a = 0; a < NMAREAS; a++)
    {
        s = (stackPos[a] - 1) & (STACKSIZE - 1);
        if (memStack[a][s] != (uint8*)p)
            continue;
        stackPos[a] = s;
        return;
    }
    assert(0);
}

#ifdef PSYQ
void debugPrint(char* message)
{
    PCwrite(-10, message, strlen(message));
}
#endif

void resetDisable(void)
{
#ifdef TODO // slave
    uint8* SMPC_SF = (uint8*)0x20100063;
    uint8* SMPC_COM = (uint8*)0x2010001f;
    const uint8 SMPC_RESDIS = 0x1a;
    while ((*SMPC_SF & 0x01) == 0x01)
        ;
    *SMPC_SF = 1;
    *SMPC_COM = SMPC_RESDIS;
    while ((*SMPC_SF & 0x01) == 0x01)
        ;
#endif
}

void resetEnable(void)
{
#ifdef TODO // slave
    uint8* SMPC_SF = (uint8*)0x20100063;
    uint8* SMPC_COM = (uint8*)0x2010001f;
    const uint8 SMPC_RESENA = 0x19;
    while ((*SMPC_SF & 0x01) == 0x01)
        ;
    *SMPC_SF = 1;
    *SMPC_COM = SMPC_RESENA;
    while ((*SMPC_SF & 0x01) == 0x01)
        ;
#endif
}

sint32 normalizeAngle(sint32 angle)
{
    while (angle > F(180))
        angle -= F(360);
    while (angle < F(-180))
        angle += F(360);
    return angle;
}

#ifndef NDEBUG
void _checkStack(char* file, sint32 line)
{
    sint32 c;
    __asm__ volatile("mov.l r15,%0\n" : "=r"((sint32)c));
    if (c < ((sint32)mystack) + 0x100)
        assertFail(file, line);
}
#endif

sint32 bitScanForward(uint32 i, sint32 start)
{
    start++;
    i = i >> start;
    while (i)
    {
        if (i & 1)
            return start;
        i = i >> 1;
        start++;
    }
    return -1;
}

sint32 bitScanBackwards(uint32 i, sint32 start)
{
    i = i << (32 - start);
    start--;
    while (i)
    {
        if (i & 0x80000000)
            return start;
        i = i << 1;
        start--;
    }
    return -1;
}

uint16 buttonMasks[8] = { PER_DGT_A, PER_DGT_B, PER_DGT_C, PER_DGT_X, PER_DGT_Y, PER_DGT_Z, PER_DGT_TL, PER_DGT_TR };
sint8 controllerConfig[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

#include "rndtab.h"
static sint32 nextRand = 0;
uint16 getNextRand(void)
{
    if (nextRand >= RANDTABLESIZE)
        nextRand = 0;
    return randTable[nextRand++];
}

void getDateTime(sint32* year, sint32* month, sint32* day, sint32* hour, sint32* min)
{
#ifdef TODO
    uint8 time;
    time = PER_GET_TIM();
    *year = (uint8)((uint16)(time[6] >> 4) * 1000 + (uint16)(time[6] & 0x0f) * 100 + (uint16)(time[5] >> 4) * 10 + (uint16)(time[5] & 0x0f) - 1980);
    *month = time[4] & 0x0f;
    *day = (time[3] >> 4) * 10 + (time[3] & 0x0f);
    *hour = (time[2] >> 4) * 10 + (time[2] & 0x0f);
    *min = (time[1] >> 4) * 10 + (time[1] & 0x0f);
#else
    *year = 1996;
    *month = 9;
    *day = 19;
    *hour = 0;
    *min = 0;
#endif
}

sint32 findWallsSector(sint32 wallNm)
{
    sint32 s;
    for (s = 0; s < level_nmSectors && wallNm > level_sector[s].lastWall; s++)
        ;
    assert(s < level_nmSectors);
    return s;
}

void displayEnable(sint32 state)
{
#ifdef TODO // display
    if (state)
        Scl_s_reg.tvmode |= 0x8000;
    else
        Scl_s_reg.tvmode &= 0x7fff;
    POKE_W(SCL_VDP2_VRAM + 0x180000, Scl_s_reg.tvmode);
    if (SclProcess == 0)
        SclProcess = 1;
#endif
}

sint32 findSectorHeight(sint32 s)
{
    MthXyz pos;
    sint32 a, b;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    a = findCeilDistance(s, &pos);
    b = findFloorDistance(s, &pos);
    return f(b - a);
}

void delay(sint32 frames)
{
    uint32 v = vtimer + frames + 1;
    while (vtimer < v)
        ;
}
