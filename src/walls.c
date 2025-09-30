#include "app.h"
#include "mth.h"
#include "level.h"
#include "sprite.h"
#include "walls.h"
#include "util.h"
#include "spr.h"
#include "print.h"
#include "pic.h"
#include "sequence.h"
#include "sruins.h"
#include "profile.h"
#include "gamestat.h"

#define WATER 1
#define WAVYWATER 0
#define RECTCLIP 1
#define ENABLEFARCLIP 0

#define TILENEARCLIP F(33)
#define MIPDIST F(256)

sint16 plaxBBymax, plaxBBxmax, plaxBBymin, plaxBBxmin;

#define MIPMAP 1

#if MIPMAP
sint16 mipBase;
#endif

static sint32 laserColor = 0;

#ifndef NDEBUG
XyInt debugLines[200][2];
sint32 nmDebugLines = 0;

void addDebugLine(sint32 x1, sint32 y1, sint32 x2, sint32 y2)
{
    assert(nmDebugLines < 200);
    debugLines[nmDebugLines][0].x = x1;
    debugLines[nmDebugLines][0].y = y1;
    debugLines[nmDebugLines][1].x = x2;
    debugLines[nmDebugLines][1].y = y2;
    nmDebugLines++;
}

void drawDebugLines(void)
{
    sint32 i;
    for (i = 0; i < nmDebugLines; i++)
    {
        EZ_line(COLOR_5 | COMPO_REP, 0xffff, debugLines[i], NULL);
    }
    nmDebugLines = 0;
}
#endif

#define SDFLAG_NEEDTOPROCESS 1
#define SDFLAG_BBVALID 2
#define SDFLAG_CACHEVALID 4
#define SDFLAG_ADDEDTOTREE 8
#define SDFLAG_DISTANCEVALID 16

#define MAXFANIN 20
typedef struct
{
    sint16 xmin, xmax, ymin, ymax;
    fix32 distance;
    sint16 ancestor[MAXFANIN];
    sint16 flags;
    sint16 spriteCommandStart; /* for slave rendered sectors only */
    sint8 nmAncestors;
    sint8 nmChildren;
} SectorDrawRecord; /* size of this structure is 60 */

sint32 nmPolys;

Sprite* autoTarget;
static sint32 bestAutoAimRating;

void project_point(MthXyz* v, XyInt* p);

#if 0
void project_point(MthXyz* v, XyInt* p)
{
    uint32 newZ;
    if (v->z <= F(1))
        newZ = F(1);
    else
        newZ = (uint32)v->z;
    {
        fix32 r;
        Set_Hardware_DivideFixed((10 << 20), newZ);
        r = Get_Hardware_Divide();
        p->x = f(MTH_Mul(r, v->x));
        p->y = -f(MTH_Mul(r, v->y));
    }
#if 0
 Set_Hardware_Divide(v->y*(-FOCALDIST>>4),newZ>>4);
 p->x = PROJECT(v->x,newZ);
 p->y = Get_Hardware_Divide();
/* p->y = -PROJECT(v->y,newZ); */
#endif
}
#else
void project_point(MthXyz *v, XyInt *p)
{
    fix32 z_clamped = (v->z > (1 << 16)) ? v->z : (1 << 16);
    fix32 divide_result = MTH_Div(F(80 << 1), z_clamped);
    p->x = f(MTH_Mul(v->x, divide_result));
    p->y = -f(MTH_Mul(v->y, divide_result));
}
#endif

#define XMIN -160
#define YMIN -110
#define XMAX 160
#define YMAX 90
sint32 clip_visible(XyInt* poly, SectorDrawRecord* s);

#if 0
__asm__
    (".align 4\n"
     ".global _clip_visible\n"
     "_clip_visible:\n"
     "lds.l r8,macl\n"
     /* load mins and maxes */
     "mov.w @r5+,r8\n" /* xmin */
     "mov.w @r5+,r7\n" /* xmax */
     "mov #0,r0\n" /* r0 is test accum */
     "mov.w @r5+,r6\n" /* ymin */
     "mov #4,r3\n" /* r3 is loop counter */
     "mov.w @r5,r5\n" /* ymax */

     /* r2 is per loop accum */
  "LLOOOOP:\n"
     "mov.w @r4+,r1\n" /* load x value */
     "cmp/ge r8,r1\n"
     "rotcl r2\n"
     "cmp/ge r1,r7\n"

     "mov.w @r4+,r1\n" /* load y value */
     "rotcl r2\n"
     "cmp/ge r6,r1\n"
     "rotcl r2\n"
     "cmp/ge r1,r5\n"
     "rotcl r2\n"

     "dt r3\n"
     "bf.s LLOOOOP\n"
     "or r2,r0\n"

     "and #15,r0\n"
     "cmp/eq #15,r0\n"
     "movt r0\n"

     "rts\n"
     "sts.l macl,r8\n"

     );
#endif

#if 1
sint32 clip_visible(XyInt* poly, SectorDrawRecord* s)
{
#if 0
 sint32 p,accum;
 accum=0;
 for (p=0;p<4;p++)
    {if (poly[p].x>=s->xmin)
	accum|=1;
     if (poly[p].y>=s->ymin)
	accum|=2;
     if (poly[p].x<=s->xmax)
	accum|=4;
     if (poly[p].y<=s->ymax)
	accum|=8;
    }
 return (accum==0xf);
#else
    /*top side clip*/
    if (poly[0].y < s->ymin && poly[1].y < s->ymin && poly[2].y < s->ymin && poly[3].y < s->ymin)
        return (0);
    /*bottom side clip*/
    if (poly[0].y > s->ymax && poly[1].y > s->ymax && poly[2].y > s->ymax && poly[3].y > s->ymax)
        return (0);
    /*left side clip*/
    if (poly[0].x < s->xmin && poly[1].x < s->xmin && poly[2].x < s->xmin && poly[3].x < s->xmin)
        return (0);
    /*right side clip*/
    if (poly[0].x > s->xmax && poly[1].x > s->xmax && poly[2].x > s->xmax && poly[3].x > s->xmax)
        return (0);
    return (1); /*polygon is visible*/
#endif
}
#endif

void clipZTile(MthXyz* pointsIn, MthXyz* pointsOut, fix32 nearClip)
{
    sint32 i;
    for (i = 0; i < 4; i++)
    {
        if (pointsIn[i].z < nearClip)
        {
            pointsOut[i].z = nearClip;
            pointsOut[i].x = pointsIn[i].x;
            pointsOut[i].y = pointsIn[i].y;
            continue;
        }
        else
        {
            pointsOut[i].x = pointsIn[i].x;
            pointsOut[i].y = pointsIn[i].y;
            pointsOut[i].z = pointsIn[i].z;
        }
    }
}

void clipZ(MthXyz* pointsIn, MthXyz* pointsOut, fix32 nearClip)
{
    sint32 i, left, right;
    fix32 ratio;
    for (i = 0; i < 4; i++)
    {
        left = i + 1;
        if (left >= 4)
            left -= 4;
        right = i - 1;
        if (right < 0)
            right += 4;
        if (pointsIn[i].z < nearClip)
        {
            pointsOut[i].z = nearClip;
            if (pointsIn[left].z > nearClip && pointsIn[right].z <= nearClip)
            {
                ratio = MTH_Div(pointsIn[left].z - nearClip, pointsIn[left].z - pointsIn[i].z);
                assert(ratio >= 0);
                assert(ratio <= F(1));
                pointsOut[i].x = pointsIn[left].x - MTH_Mul((pointsIn[left].x - pointsIn[i].x), ratio);
                /* out is between i and left */
                assert((pointsOut[i].x >= pointsIn[left].x && pointsOut[i].x <= pointsIn[i].x) || (pointsOut[i].x <= pointsIn[left].x && pointsOut[i].x >= pointsIn[i].x));
                pointsOut[i].y = pointsIn[left].y - MTH_Mul((pointsIn[left].y - pointsIn[i].y), ratio);
                assert((pointsOut[i].y >= pointsIn[left].y && pointsOut[i].y <= pointsIn[i].y) || (pointsOut[i].y <= pointsIn[left].y && pointsOut[i].y >= pointsIn[i].y));
            }
            else
            {
                if (pointsIn[right].z > nearClip && pointsIn[left].z <= nearClip)
                {
                    ratio = MTH_Div(pointsIn[right].z - nearClip, pointsIn[right].z - pointsIn[i].z);
                    assert(ratio >= 0);
                    assert(ratio <= F(1));
                    pointsOut[i].x = pointsIn[right].x - MTH_Mul((pointsIn[right].x - pointsIn[i].x), ratio);
                    /* out is between i and left */
                    assert((pointsOut[i].x >= pointsIn[right].x && pointsOut[i].x <= pointsIn[i].x) || (pointsOut[i].x <= pointsIn[right].x && pointsOut[i].x >= pointsIn[i].x));
                    pointsOut[i].y = pointsIn[right].y - MTH_Mul((pointsIn[right].y - pointsIn[i].y), ratio);
                    assert((pointsOut[i].y >= pointsIn[right].y && pointsOut[i].y <= pointsIn[i].y) || (pointsOut[i].y <= pointsIn[right].y && pointsOut[i].y >= pointsIn[i].y));
                }
                else
                {
                    pointsOut[i].x = pointsIn[i].x;
                    pointsOut[i].y = pointsIn[i].y;
                }
            }
        }
        else
        {
            pointsOut[i].x = pointsIn[i].x;
            pointsOut[i].y = pointsIn[i].y;
            pointsOut[i].z = pointsIn[i].z;
        }
    }
}

#define goodPoint(p) (greater ? ((fix32*)(p))[clipAxis] > clipLine : ((fix32*)(p))[clipAxis] < clipLine)

void clipZSub(sint32 clipAxis, fix32 clipLine, sint32 greater, MthXyz* pointsIn, fix32* shadeIn, sint32 nmInQuads, MthXyz* pointsOut, fix32* shadeOut, sint32* nmOutQuads)
{
    sint32 q, p, nmRing;
    sint32 next;
    sint32 i;
    MthXyz ring[10];
    fix32 shadeRing[10];
    sint32 nmOutPoints;

    *nmOutQuads = 0;
    nmOutPoints = 0;
    for (q = 0; q < nmInQuads; q++)
    {
        nmRing = 0;
        for (p = 0; p < 4; p++)
        { /* consider the lines one at a time */
            next = p + 1;
            if (next >= 4)
                next -= 4;

            if ((i = goodPoint(pointsIn + p)) != 0)
            {
                shadeRing[nmRing] = shadeIn[p];
                ring[nmRing++] = pointsIn[p];
                assert(nmRing < 10);
            }

            /* add intersection point */
            if (i != goodPoint(pointsIn + next))
            {
                fix32 inter[3];
                fix32 *thisA, *nextA;
                fix32 ratio;
                thisA = (fix32*)(pointsIn + p);
                nextA = (fix32*)(pointsIn + next);
                inter[clipAxis] = clipLine;
                ratio = MTH_Div(nextA[clipAxis] - clipLine, nextA[clipAxis] - thisA[clipAxis]);
#if 0
	     if (ratio<0)
		{dPrint("ratio %d\n %d %d %d\n",ratio,
			nextA[clipAxis],clipLine,thisA[clipAxis]);
		 dPrint("%d",clipAxis);
		}
#endif
                assert(ratio >= 0); /* can crash here. Why? */
                assert(ratio <= F(1));

                for (i = 0; i < 3; i++)
                {
                    if (i == clipAxis)
                        continue;
                    inter[i] = nextA[i] - MTH_Mul(nextA[i] - thisA[i], ratio);
                }
                ring[nmRing].x = inter[0];
                ring[nmRing].y = inter[1];
                ring[nmRing].z = inter[2];
                shadeRing[nmRing] = shadeIn[next] - MTH_Mul(shadeIn[next] - shadeIn[p], ratio);
                nmRing++;
                assert(nmRing < 10);
            }
        }
        /* now look at the points in the ring and translate them into quads */
        switch (nmRing)
        {
            case 0:
                break;
            case 3:
                for (i = 0; i < nmRing; i++)
                {
                    shadeOut[nmOutPoints] = shadeRing[i];
                    pointsOut[nmOutPoints++] = ring[i];
                }
                shadeOut[nmOutPoints] = shadeRing[0];
                pointsOut[nmOutPoints++] = ring[0];
                (*nmOutQuads)++;
                break;
            case 4:
                for (i = 0; i < nmRing; i++)
                {
                    shadeOut[nmOutPoints] = shadeRing[i];
                    pointsOut[nmOutPoints++] = ring[i];
                }
                (*nmOutQuads)++;
                break;
            case 5:
                for (i = 0; i < 4; i++)
                {
                    shadeOut[nmOutPoints] = shadeRing[i];
                    pointsOut[nmOutPoints++] = ring[i];
                }
                (*nmOutQuads)++;
                assert(*nmOutQuads < 12);
                shadeOut[nmOutPoints] = shadeRing[3];
                pointsOut[nmOutPoints++] = ring[3];
                shadeOut[nmOutPoints] = shadeRing[4];
                pointsOut[nmOutPoints++] = ring[4];
                shadeOut[nmOutPoints] = shadeRing[0];
                pointsOut[nmOutPoints++] = ring[0];
                shadeOut[nmOutPoints] = shadeRing[0];
                pointsOut[nmOutPoints++] = ring[0];
                (*nmOutQuads)++;
                break;
            default:
#if 0
#ifndef NDEBUG
	    dPrint("nmRing=%d\n",nmRing);
	    for (p=0;p<4;p++)
	       {dPrint(" p(%d,%d,%d)\n",
		       pointsIn[p].x,
		       pointsIn[p].y,
		       pointsIn[p].z);
		dPrint("axis %d   line %d  greater %d\n",clipAxis,
		       clipLine,greater);
	       }
#endif
#endif
                assert(0);
        }
        pointsIn += 4;
        shadeIn += 4;
        assert(*nmOutQuads < 12);
    }
}

#define WATERCOLOR (RGB(4, 4, 8))
#define GETWATERBRIGHT(x, z) ((sint32)(waterBright[(((uint32)(x)) >> 22) & 0xf][(((uint32)(z)) >> 22) & 0xf]))
static sint8 waterBright[16][16];
static sint32 sawWater;
static sint32 waterPos[16][16];
static sint32 waterVel[16][16];
void initWater(void)
{
    sint32 x, y, i;
    extern uint16 randTable[];
    i = 0;
    for (y = 0; y < 16; y++)
        for (x = 0; x < 16; x++)
        {
            waterPos[y][x] = randTable[i++] << 4;
            waterVel[y][x] = 0;
        }
    /* waterVel[0][1]=F(1000);
     waterVel[2][1]=F(1000);
     waterVel[1][1]=F(1000);
     waterVel[1][0]=F(1000);
     waterVel[1][2]=F(1000);

     waterVel[7][4]=F(-1000);
     waterVel[9][4]=F(-1000);
     waterVel[8][4]=F(-1000);
     waterVel[8][3]=F(-1000);
     waterVel[8][5]=F(-1000);*/
}

void stepWater(void)
{
    sint32 x, y, b;
    laserColor += 2;
    if (laserColor > 61)
        laserColor = 0;
    if (camera->flags & SPRITEFLAG_UNDERWATER)
        sawWater = 5;
    if (!sawWater)
        return;
    sawWater--;
    for (y = 0; y < 16; y++)
        for (x = 0; x < 16; x++)
        {
            waterPos[y][x] += waterVel[y][x] >> 8 /*8*/;
            b = f(waterPos[y][x]) + 15;
            if (b < 0)
                b = 0;
            if (b > 31)
                b = 31;
            waterBright[y][x] = b;
        }
    for (y = 0; y < 16; y++)
        for (x = 0; x < 16; x++)
        {
            waterVel[y][x] += waterPos[(y + 1) & 0xf][x & 0xf] + waterPos[(y - 1) & 0xf][x & 0xf] + waterPos[y & 0xf][(x + 1) & 0xf] + waterPos[y & 0xf][(x - 1) & 0xf] - (waterPos[y][x] << 2);
        }
}

#define LIGHT
#define BETTERLIGHT 0
#define MAXNMLIGHTSOURCES 15
#define LIGHTRADIUS 256

static Sprite* lightSource[MAXNMLIGHTSOURCES];
static sint32 nmLights, delayNmLights;
static MthXyz tLightPos[MAXNMLIGHTSOURCES];
static sint32 lColor[MAXNMLIGHTSOURCES][3];
static sint32 delayColor[MAXNMLIGHTSOURCES][3];
static sint8 lightColorChanged = 0, lightsDeleted = 0;
static sint8 delayDeleteLight[MAXNMLIGHTSOURCES];
static void lightInit(void)
{
    sint32 i;
    for (i = 0; i < MAXNMLIGHTSOURCES; i++)
    {
        lightSource[i] = NULL;
        delayDeleteLight[i] = 0;
    }
    nmLights = 0;
    delayNmLights = 0;
    lightColorChanged = 0;
    lightsDeleted = 0;
}

void addLight(Sprite* s, sint32 r, sint32 g, sint32 b)
{
    if (delayNmLights >= MAXNMLIGHTSOURCES)
        return;
    assert(delayNmLights < MAXNMLIGHTSOURCES);
    lColor[delayNmLights][0] = r;
    lColor[delayNmLights][1] = g;
    lColor[delayNmLights][2] = b;
    delayColor[delayNmLights][0] = r;
    delayColor[delayNmLights][1] = g;
    delayColor[delayNmLights][2] = b;
    lightSource[delayNmLights] = s;
    delayNmLights++;
}

void changeLightColor(Sprite* s, sint32 r, sint32 g, sint32 b)
{
    sint32 i;
    for (i = 0; i < MAXNMLIGHTSOURCES && lightSource[i] != s; i++)
        ;
    if (i == MAXNMLIGHTSOURCES)
        return;
    delayColor[i][0] = r;
    delayColor[i][1] = g;
    delayColor[i][2] = b;
    lightColorChanged = 1;
}

void removeLight(Sprite* s)
{
    sint32 i;
    for (i = 0; i < MAXNMLIGHTSOURCES && lightSource[i] != s; i++)
        ;
    if (i == MAXNMLIGHTSOURCES)
        return;
    lightsDeleted = 1;
    delayDeleteLight[i] = 1;
}

void updateLights(void)
{
    sint32 i, j;
    nmLights = delayNmLights;

    if (lightColorChanged)
    {
        lightColorChanged = 0;
        for (i = 0; i < MAXNMLIGHTSOURCES; i++)
        {
            lColor[i][0] = delayColor[i][0];
            lColor[i][1] = delayColor[i][1];
            lColor[i][2] = delayColor[i][2];
        }
    }
    if (lightsDeleted)
    {
        for (i = 0, j = 0; j < nmLights; j++)
        {
            if (!delayDeleteLight[j])
            {
                if (i != j)
                {
                    lightSource[i] = lightSource[j];
                    lColor[i][0] = lColor[j][0];
                    lColor[i][1] = lColor[j][1];
                    lColor[i][2] = lColor[j][2];
                    delayColor[i][0] = lColor[i][0];
                    delayColor[i][1] = lColor[i][1];
                    delayColor[i][2] = lColor[i][2];
                }
                i++;
            }
            else
                delayDeleteLight[j] = 0;
        }
        nmLights = i;
        delayNmLights = i;
        lightsDeleted = 0;
    }
}

sint32 nmWallLights;
static sint8 wallLightP[MAXNMLIGHTSOURCES];
#if BETTERLIGHT
static sint32 wallLightDist[MAXNMLIGHTSOURCES];
#endif
static void buildLightList(sWallType* wall)
{
    sint32 l;
    fix32 dist;
    MthXyz wallP;
    getVertex(wall->v[0], &wallP);
    nmWallLights = 0;
    for (l = 0; l < nmLights; l++)
    { /* find distance from light to wall's plane */
        dist = (f(lightSource[l]->pos.x - wallP.x)) * wall->normal[0] + (f(lightSource[l]->pos.y - wallP.y)) * wall->normal[1] + (f(lightSource[l]->pos.z - wallP.z)) * wall->normal[2];
        if (
#if BETTERLIGHT
            dist <= 0
#else
            dist < -F(LIGHTRADIUS)
#endif
            || dist > F(LIGHTRADIUS))
            wallLightP[l] = 0;
        else
        {
            wallLightP[l] = 1;
#if BETTERLIGHT
            wallLightDist[l] = MTH_Mul(dist >> 11, dist);
#endif
            nmWallLights++;
        }
    }
}

static sint32 snmWallLights;
static sint8 swallLightP[MAXNMLIGHTSOURCES];
#if BETTERLIGHT
static sint32 swallLightDist[MAXNMLIGHTSOURCES];
#endif
static void sbuildLightList(sWallType* wall)
{
    sint32 l;
    fix32 dist;
    MthXyz wallP;
    getVertex(wall->v[0], &wallP);
    snmWallLights = 0;
    for (l = 0; l < nmLights; l++)
    { /* find distance from light to wall's plane */
        dist = (f(lightSource[l]->pos.x - wallP.x)) * wall->normal[0] + (f(lightSource[l]->pos.y - wallP.y)) * wall->normal[1] + (f(lightSource[l]->pos.z - wallP.z)) * wall->normal[2];
        if (
#if BETTERLIGHT
            dist <= 0
#else
            dist < -F(LIGHTRADIUS)
#endif
            || dist > F(LIGHTRADIUS))
            swallLightP[l] = 0;
        else
        {
            swallLightP[l] = 1;
#if BETTERLIGHT
            swallLightDist[l] = MTH_Mul(dist >> 11, dist);
#endif
            snmWallLights++;
        }
    }
}

#define NEARCLIP F(32)
#define SECTORBNDRYNEARCLIP F(-1)
#define FARCLIP F(1024)
#define FARCLIP2 10
/*#define FARCLIP F(256)
  #define FARCLIP2 8 */

static sint32 wavyIndex = 0;

uint16 getLight(sint8 vlight, MthXyz* pos)
{
    sint32 r, g, b, i, light;
    if (wavyIndex)
    {
        vlight += ((*(((sint8*)waterBright) + wavyIndex)) - 20) >> 1;
        if (vlight > 31)
            vlight = 31;
        if (vlight < 0)
            vlight = 0;
        wavyIndex = (wavyIndex + 7) & 0xff;
        if (!wavyIndex)
            wavyIndex++;
    }
    if (!nmWallLights)
        return greyTable[(sint32)vlight];
    r = g = b = vlight;
    for (i = 0; i < nmLights; i++)
    {
        sint32 dist;
        if (!wallLightP[i])
            continue;
        dist = (f(pos->x - tLightPos[i].x) * f(pos->x - tLightPos[i].x) + f(pos->y - tLightPos[i].y) * f(pos->y - tLightPos[i].y) + f(pos->z - tLightPos[i].z) * f(pos->z - tLightPos[i].z));
        light = LIGHTRADIUS * LIGHTRADIUS - dist;
        if (light <= 0)
            continue;
#if BETTERLIGHT
        if (dist > 10)
            light = (light * (wallLightDist[i] / dist)) >> 16;
        else
#else
        light = light >> 11;
#endif
            if (light - lColor[i][0] > 0)
            r += light - lColor[i][0];
        if (light - lColor[i][1] > 0)
            g += light - lColor[i][1];
        if (light - lColor[i][2] > 0)
            b += light - lColor[i][2];
    }
    if (r > 31)
        r = 31;
    if (g > 31)
        g = 31;
    if (b > 31)
        b = 31;
    return RGB(r, g, b);
}

static sint32 sWavyIndex = 0;
uint16 sgetLight(sint8 vlight, MthXyz* pos)
{
    sint32 r, g, b, i, light, dist;
    if (sWavyIndex)
    {
        vlight += ((*(((sint8*)waterBright) + sWavyIndex)) - 20) >> 1;
        if (vlight > 31)
            vlight = 31;
        if (vlight < 0)
            vlight = 0;
        sWavyIndex = (sWavyIndex + 7) & 0xff;
        if (!sWavyIndex)
            sWavyIndex++;
    }
    if (!snmWallLights)
        return greyTable[(sint32)vlight];
    r = g = b = vlight;
    for (i = 0; i < nmLights; i++)
    {
        if (!swallLightP[i])
            continue;
        dist = (f(pos->x - tLightPos[i].x) * f(pos->x - tLightPos[i].x) + f(pos->y - tLightPos[i].y) * f(pos->y - tLightPos[i].y) + f(pos->z - tLightPos[i].z) * f(pos->z - tLightPos[i].z));
        light = LIGHTRADIUS * LIGHTRADIUS - dist;
        if (light <= 0)
            continue;
#if BETTERLIGHT
        if (dist > 10)
            light = (light * (swallLightDist[i] / dist)) >> 16;
        else
#else
        light = light >> 11;
#endif
            if (light - lColor[i][0] > 0)
            r += light - lColor[i][0];
        if (light - lColor[i][1] > 0)
            g += light - lColor[i][1];
        if (light - lColor[i][2] > 0)
            b += light - lColor[i][2];
    }
    if (r > 31)
        r = 31;
    if (g > 31)
        g = 31;
    if (b > 31)
        b = 31;
    return RGB(r, g, b);
}

#define MAXSUBS 12
void drawClippedFace(sFaceType* face, fix32* shades, MthMatrix* view, sint32 wallStartVtx)
{
    sint32 i, z;
    sint32 buff1Size, buff2Size;
    MthXyz buff1[MAXSUBS * 4];
    MthXyz buff2[MAXSUBS * 4];
    fix32 shade1[MAXSUBS * 4];
    fix32 shade2[MAXSUBS * 4];
    XyInt poly[4];
    struct gourTable gtable;

    for (i = 0; i < 4; i++)
    {
        getVertex(face->v[i] + wallStartVtx, buff1 + i);
        MTH_CoordTrans(view, buff1 + i, buff2 + i);
    }
    for (i = 0; i < 4; i++)
        shade2[i] = F(shades[i]);
    clipZSub(2, F(5), 1, buff2, shade2, 1, buff1, shade1, &buff1Size);
    for (i = 0; i < buff1Size * 4; i++)
    {
#if 1
        XyInt xy;
        project_point(buff1 + i, &xy);
        buff2[i].x = F(xy.x);
        buff2[i].y = F(xy.y);
#else
        buff2[i].x = F(PROJECT(buff1[i].x, buff1[i].z));
        buff2[i].y = -F(PROJECT(buff1[i].y, buff1[i].z));
#endif
        shade2[i] = shade1[i];
        if (buff2[i].x > F(2000))
            buff2[i].x = F(2000);
        if (buff2[i].x < F(-2000))
            buff2[i].x = F(-2000);
        if (buff2[i].y > F(2000))
            buff2[i].y = F(2000);
        if (buff2[i].y < F(-2000))
            buff2[i].y = F(-2000);
        buff2[i].z = 0;
    }
    buff2Size = buff1Size;
    clipZSub(0, F(-160), 1, buff2, shade2, buff2Size, buff1, shade1, &buff1Size);
    assert(buff1Size < MAXSUBS);

    clipZSub(0, F(160), 0, buff1, shade1, buff1Size, buff2, shade2, &buff2Size);
    assert(buff2Size < MAXSUBS);

    clipZSub(1, F(-120), 1, buff2, shade2, buff2Size, buff1, shade1, &buff1Size);
    assert(buff1Size < MAXSUBS);

    clipZSub(1, F(120), 0, buff1, shade1, buff1Size, buff2, shade2, &buff2Size);
    assert(buff2Size < MAXSUBS);

    for (i = 0; i < buff2Size; i++)
    {
        for (z = 0; z < 4; z++)
        {
            poly[z].x = f(buff2[z + i * 4].x);
            poly[z].y = f(buff2[z + i * 4].y);
            assert(poly[z].x > -161);
            assert(poly[z].y > -121);
            assert(poly[z].x < 161);
            assert(poly[z].y < 121);
            assert(f(shade2[z + i * 4]) >= 0);
            assert(f(shade2[z + i * 4]) < 32);
            gtable.entry[z] = greyTable[f(shade2[z + i * 4])];
        }
        EZ_polygon(DRAW_GOURAU | DRAW_MESH | ECD_DISABLE | SPD_DISABLE, WATERCOLOR, poly, &gtable);
    }
}

#if 0
void drawWater(sWallType *theWall,MthXyz *coords)
(sint32 i,z,field;
 sint32 buff1Size,buff2Size;
 MthXyz buff1[MAXSUBS*4];
 MthXyz buff2[MAXSUBS*4];
 fix32 shade1[MAXSUBS*4];
 fix32 shade2[MAXSUBS*4];
 XyInt poly[4];
 struct gourTable gtable;

#if 0
 {static sint32 wave=0;
  static fix32 pos[4]={F(10),F(7),F(4),-F(5)};
  static fix32 vel[4]={0,0,0,0};
  sint32 i;
  for (i=0;i<4;i++)
     pos[i]+=vel[i]>>8;
  vel[0]+=-4*pos[0]+pos[1]+pos[3];
  vel[1]+=-4*pos[1]+pos[0]+pos[2];
  vel[2]+=-4*pos[2]+pos[1]+pos[3];
  vel[3]+=-4*pos[3]+pos[0]+pos[2];
  for (i=0;i<4;i++)
     shade2[i]=pos[i]+F(15);
 }
#endif

 if (theWall->normal[1]!=0)
    {/* water */
     WaveFace *f=level_waveFace+(sint32)theWall->object;
     for (i=0;i<4;i++)
	{shade2[i]=level_waveVert[f->connect[i]].pos+F(15);
	 if (shade2[i]<0)
	    shade2[i]=0;
	 if (shade2[i]>=F(31))
	    shade2[i]=F(31);
	 assert(f(shade2[i])>=0);
	 assert(f(shade2[i])<32);
	}
     sawWater=6;
     field=0;
    }
 else
    {/* force field */
     for (i=0;i<4;i++)
	shade2[i]=force[i];
     field=1;
    }

 clipZSub(2,F(5),1,
	  coords,shade2,1,
	  buff1,shade1,&buff1Size);

 for (i=0;i<buff1Size*4;i++)
    {buff2[i].x=F(PROJECT(buff1[i].x,buff1[i].z));
     buff2[i].y=-F(PROJECT(buff1[i].y,buff1[i].z));
     shade2[i]=shade1[i];
    }
 buff2Size=buff1Size;

 clipZSub(0,F(-160),1,
	  buff2,shade2,buff2Size,
	  buff1,shade1,&buff1Size);
 assert(buff1Size<MAXSUBS);

 clipZSub(0,F(160),0,
	  buff1,shade1,buff1Size,
	  buff2,shade2,&buff2Size);
 assert(buff2Size<MAXSUBS);

 clipZSub(1,F(-120),1,
	  buff2,shade2,buff2Size,
	  buff1,shade1,&buff1Size);
 assert(buff1Size<MAXSUBS);

 clipZSub(1,F(120),0,
	  buff1,shade1,buff1Size,
	  buff2,shade2,&buff2Size);
 assert(buff2Size<MAXSUBS);

 for (i=0;i<buff2Size;i++)
    {for (z=0;z<4;z++)
	{poly[z].x=f(buff2[z+i*4].x);
	 poly[z].y=f(buff2[z+i*4].y);
	 assert(poly[z].x>-161);
	 assert(poly[z].y>-121);
	 assert(poly[z].x<161);
	 assert(poly[z].y<121);
	 assert(f(shade2[z+i*4])>=0);
	 assert(f(shade2[z+i*4])<32);
	 gtable.entry[z]=greyTable[f(shade2[z+i*4])];
	}
     EZ_polygon(DRAW_MESH|ECD_DISABLE|SPD_DISABLE,
		field?RGB(15,10,0):WATERCOLOR,poly,&gtable);
    }
}
#endif

#if 0
void drawPlax(sWallType *theWall,MthXyz *coords)
(sint32 i,z;
 sint32 buff1Size,buff2Size;
 MthXyz buff1[MAXSUBS*4];
 MthXyz buff2[MAXSUBS*4];
 fix32 shade1[MAXSUBS*4];
 fix32 shade2[MAXSUBS*4];
 XyInt poly[4];

 clipZSub(2,F(5),1,
	  coords,shade2,1,
	  buff1,shade1,&buff1Size);

 for (i=0;i<buff1Size*4;i++)
    {buff2[i].x=F(PROJECT(buff1[i].x,buff1[i].z));
     buff2[i].y=-F(PROJECT(buff1[i].y,buff1[i].z));
     shade2[i]=shade1[i];
    }
 buff2Size=buff1Size;

 clipZSub(0,F(-160),1,
	  buff2,shade2,buff2Size,
	  buff1,shade1,&buff1Size);
 assert(buff1Size<MAXSUBS);

 clipZSub(0,F(160),0,
	  buff1,shade1,buff1Size,
	  buff2,shade2,&buff2Size);
 assert(buff2Size<MAXSUBS);

 clipZSub(1,F(-120),1,
	  buff2,shade2,buff2Size,
	  buff1,shade1,&buff1Size);
 assert(buff1Size<MAXSUBS);

 clipZSub(1,F(120),0,
	  buff1,shade1,buff1Size,
	  buff2,shade2,&buff2Size);
 assert(buff2Size<MAXSUBS);

 for (i=0;i<buff2Size;i++)
    {for (z=0;z<4;z++)
	{poly[z].x=f(buff2[z+i*4].x);
	 poly[z].y=f(buff2[z+i*4].y);
	 assert(poly[z].x>-161);
	 assert(poly[z].y>-121);
	 assert(poly[z].x<161);
	 assert(poly[z].y<121);
	}
     EZ_polygon(ECD_DISABLE|SPD_DISABLE,0x0000,poly,NULL);
    }
}
#endif

#define MAXVPERWALL 700
typedef struct 
{
    sint16 x, y;
    uint16 light;
    /* sint8 clip;  clipped if (light&0x8000) */
} VCALC;

static sint8 pattern[][4] = { { 0, 1, 2, 3 }, { 1, 2, 3, 0 }, { 2, 3, 0, 1 }, { 3, 0, 1, 2 }, { 0, 3, 2, 1 }, { 1, 0, 3, 2 }, { 2, 1, 0, 3 }, { 3, 2, 1, 0 } };

void rectTransform(fix32 wx, fix32 wy, fix32 wz, sint32 light_idx, sint32 h, sint32 w, fix32 px, fix32 py, fix32 pz, fix32 hx, fix32 hy, fix32 hz, VCALC *output, uint16 (*lightFunc)(sint8 vLightIndex, MthXyz *pos))
{
    sint32 i, divide_result, light, depth;
    fix32 z_clamped;
    uint8 *light_array = (uint8*)(level_vertexLight + light_idx);
    uint16 light_value;

    while (h > 0)
    {
        fix32 curr_x = px, curr_y = py, curr_z = pz;
        for (i = w; i > 0; i--)
        {
            z_clamped = (curr_z > F(33)) ? curr_z : F(33);

            divide_result = MTH_Div(F(80 << 1), z_clamped);

            light = *light_array++;
            depth = z_clamped >> 24;
            light = (depth > light) ? 0 : light - depth;

            if (lightFunc)
            {
                MthXyz pos;
                pos.x = curr_x;
                pos.y = curr_y;
                pos.z = curr_z;
                light_value = lightFunc(light, &pos);
            }
            else
            {
                light_value = greyTable[light << 1];
            }

            if (curr_z > F(33))
            {
                light_value &= ~0x8000;
            }

            output->x = (sint16)(f(MTH_Mul(curr_x, divide_result)));
            output->y = (sint16)(-f(MTH_Mul(curr_y, divide_result)));
            output->light = light_value;
            output++;

            curr_x += wx;
            curr_y += wy;
            curr_z += wz;
        }

        px += hx;
        py += hy;
        pz += hz;
        h--;
    }
}

void normTransform(sVertexType *vertex, MthMatrix *viewMatrix, sint32 nmVert, VCALC *output, uint16 (*lightFunc)(sint8 vLightIndex, MthXyz *pos))
{
    MthXyz src, dst;
    sint32 z, x, y, depth, light, z_clamped, divide_result;
    uint16 light_value;
    sVertexType *end = vertex + nmVert;

    while (vertex < end)
    {
        src.x = F(vertex->x);
        src.y = F(vertex->y);
        src.z = F(vertex->z);
        light = vertex->light;
        vertex++;

        MTH_CoordTrans(viewMatrix, &src, &dst);

        x = dst.x;
        y = dst.y;
        z = dst.z;

        z_clamped = (z > (33 << 16)) ? z : (33 << 16);

        divide_result = MTH_Div(F(80 << 1), z_clamped);

        depth = z_clamped >> 24;
        light = (depth > light) ? 0 : light - depth;

        if (lightFunc) {
            dst.z = z_clamped;
            light_value = lightFunc(light, &dst);
        } else {
            light_value = greyTable[light << 1];
        }

        if (z > (33 << 16))
        {
            light_value &= ~0x8000;
        }

        output->x = (sint16)(f(MTH_Mul(x, divide_result)));
        output->y = (sint16)(-f(MTH_Mul(y, divide_result)));
        output->light = light_value;

        output++;
    }
}

void drawRectWall(sWallType* theWall, MthXyz* coords, SectorDrawRecord* s)
{
    MthXyz vWidth, vHeight;
    XyInt poly[4];
    sint32 w, h, clip;
    sint32 light;
    sint32 tex, row1, row2;
    register sint8* ppattern;
    sint32 width = theWall->tileLength;
    sint32 height = theWall->tileHeight;
    struct gourTable gtable;
    VCALC vCalc[MAXVPERWALL];
#if MIPMAP
    sint32 tileBias;
#endif

    assert(width * height < MAXVPERWALL);

#if MIPMAP
#ifdef TODO // added width > 1 && height > 1 check
#endif
    if (width > 1 && height > 1 && currentState.desiredWeapon && coords[0].z > MIPDIST && coords[1].z > MIPDIST && coords[2].z > MIPDIST && coords[3].z > MIPDIST)
    {
        width >>= 1;
        height >>= 1;
        tileBias = mipBase;
    }
    else
        tileBias = 0;
#endif

    buildLightList(theWall);

    Set_Hardware_Divide(coords[1].x - coords[0].x, width);
    vWidth.y = (coords[1].y - coords[0].y) / width;
    vWidth.x = Get_Hardware_Divide();

    Set_Hardware_Divide(coords[1].z - coords[0].z, width);
    vHeight.x = (coords[2].x - coords[1].x) / height;
    vWidth.z = Get_Hardware_Divide();

    Set_Hardware_Divide(coords[2].y - coords[1].y, height);
    vHeight.z = (coords[2].z - coords[1].z) / height;
    vHeight.y = Get_Hardware_Divide();

    light = theWall->firstLight;

    rectTransform(vWidth.x, vWidth.y, vWidth.z, light, height + 1, width + 1, coords[0].x, coords[0].y, coords[0].z, vHeight.x, vHeight.y, vHeight.z, vCalc, (nmWallLights || wavyIndex) ? &getLight : NULL);

    tex = theWall->textures;
    row1 = 0;
    row2 = width + 1;
    pushProfile("2nd Half");
    for (h = 0; h < height; h++)
    {
        for (w = 0; w < width; w++)
        {
            clip = 0x8000;
            assert(level_texture[tex] < 8);
            ppattern = pattern[(sint32)level_texture[tex++]];
            gtable.entry[(sint32)*ppattern] = vCalc[row1 + w].light;
            clip &= vCalc[row1 + w].light;
            poly[(sint32)*ppattern].x = vCalc[row1 + w].x;
            poly[(sint32)*ppattern].y = vCalc[row1 + w].y;
            ppattern++;

            gtable.entry[(sint32)*ppattern] = vCalc[row1 + w + 1].light;
            clip &= vCalc[row1 + w + 1].light;
            poly[(sint32)*ppattern].x = vCalc[row1 + w + 1].x;
            poly[(sint32)*ppattern].y = vCalc[row1 + w + 1].y;
            ppattern++;

            gtable.entry[(sint32)*ppattern] = vCalc[row2 + w + 1].light;
            clip &= vCalc[row2 + w + 1].light;
            poly[(sint32)*ppattern].x = vCalc[row2 + w + 1].x;
            poly[(sint32)*ppattern].y = vCalc[row2 + w + 1].y;
            ppattern++;

            gtable.entry[(sint32)*ppattern] = vCalc[row2 + w].light;
            clip &= vCalc[row2 + w].light;
            poly[(sint32)*ppattern].x = vCalc[row2 + w].x;
            poly[(sint32)*ppattern].y = vCalc[row2 + w].y;

            if (clip || !clip_visible(poly, s))
            {
                tex++;
                continue;
            }

            assert(getPicClass(level_texture[tex]) == TILE16BPP);
#if 0
	 EZ_distSpr(DIR_NOREV,
		    UCLPIN_ENABLE|COLOR_5|HSS_ENABLE|ECD_DISABLE|
		    DRAW_GOURAU,
		    0,mapPic(level_texture[tex]),poly,&gtable);
#endif
            EZ_specialDistSpr2(mapPic(level_texture[tex]
#if MIPMAP
                                   + tileBias
#endif
                                   ),
                poly, &gtable);
            nmPolys++;
            tex++;
        }
        row1 += width + 1;
        row2 += width + 1;
    }
    popProfile();
}

void drawWall(sWallType* wall, MthMatrix* view, SectorDrawRecord* s)
{
    sint32 f, i, v, clip;
    XyInt poly[4];
    struct gourTable gtable;
    VCALC vCalc[MAXVPERWALL];
#ifndef NDEBUG
    sint32 maxV;
#endif

#ifdef LIGHT
    buildLightList(wall);
#endif

    assert(wall->lastVertex - wall->firstVertex < MAXVPERWALL);
    v = 0;
    assert(wall->lastVertex - wall->firstVertex < MAXVPERWALL);
    normTransform(level_vertex + wall->firstVertex, view, wall->lastVertex - wall->firstVertex + 1, vCalc, (nmWallLights || wavyIndex) ? &getLight : NULL);

#ifndef NDEBUG
    maxV = wall->lastVertex - wall->firstVertex + 1;
    assert(maxV < MAXVPERWALL);
    assert(wall->firstFace >= 0);
    assert(s->xmin >= -160 && s->ymin >= -120 && s->xmax <= 160 && s->ymax <= 120);
#endif
    for (f = wall->firstFace; f <= wall->lastFace; f++)
    {
        clip = 0x8000;
        for (i = 0; i < 4; i++)
        {
            v = level_face[f].v[i];
            assert(v >= 0);
            //assert(v < maxV);
            gtable.entry[i] = vCalc[v].light;
            clip &= vCalc[v].light;
            poly[i].x = vCalc[v].x;
            poly[i].y = vCalc[v].y;
        }
        if (clip || !clip_visible(poly, s))
            continue;

        assert(getPicClass(level_face[f].tile) == TILE16BPP);
#if 0
     EZ_distSpr(DIR_NOREV,
		UCLPIN_ENABLE|COLOR_5|HSS_ENABLE|ECD_DISABLE|DRAW_GOURAU,
		0,mapPic(level_face[f].tile),poly,&gtable);
#endif
        EZ_specialDistSpr2(mapPic(level_face[f].tile), poly, &gtable);
        nmPolys++;
    }
}

void drawWaterSurface(sWallType* wall, MthMatrix* view, SectorDrawRecord* s)
{
    sint32 f, i, v, clip, clip1;
    sint32 light;
    MthXyz original;
    XyInt poly[4];
    MthXyz tformed;
    struct gourTable gtable;
    VCALC vCalc[MAXVPERWALL];
#ifndef NDEBUG
    sint32 maxV;
#endif

    sawWater = 5;
    assert(!(wall->flags & WALLFLAG_PARALLELOGRAM));

    if (wall->firstFace < 0)
        return;

    assert(wall->lastVertex - wall->firstVertex < MAXVPERWALL);
    v = 0;
    light = wall->firstLight;
    assert(wall->lastVertex - wall->firstVertex < MAXVPERWALL);
    for (i = wall->firstVertex; i <= wall->lastVertex; i++)
    {
        getVertex(i, &original);
        vCalc[v].light = greyTable[GETWATERBRIGHT(original.x, original.z)];
#if WAVYWATER
        /* wavy water */
        original.y += F(GETWATERBRIGHT(original.x, original.z) - 16);
#endif
        MTH_CoordTrans(view, &original, &tformed);
        if (tformed.z > F(20))
            vCalc[v].light &= ~0x8000;
        project_point(&tformed, poly);
        vCalc[v].x = poly[0].x;
        vCalc[v].y = poly[0].y;
        v++;
        assert(v < MAXVPERWALL);
    }
#ifndef NDEBUG
    maxV = v;
    assert(maxV < MAXVPERWALL);
    assert(wall->firstFace >= 0);
    assert(s->xmin >= -160 && s->ymin >= -120 && s->xmax <= 160 && s->ymax <= 120);
#endif
    for (f = wall->firstFace; f <= wall->lastFace; f++)
    {
        clip1 = 0;
        clip = 0x8000;
        for (i = 0; i < 4; i++)
        {
            v = level_face[f].v[i];
            assert(v >= 0);
            assert(v < maxV);
            gtable.entry[i] = vCalc[v].light;
            clip &= vCalc[v].light;
            clip1 |= vCalc[v].light;
            poly[i].x = vCalc[v].x;
            poly[i].y = vCalc[v].y;
        }

        if (clip1 & 0x8000)
        { /* do real polygon clipping */
            fix32 shades[4];
            for (i = 0; i < 4; i++)
            {
                v = level_face[f].v[i];
                shades[i] = vCalc[v].light & 0x1f;
            }
            drawClippedFace(level_face + f, shades, view, wall->firstVertex);
            continue;
        }

        if (clip || !clip_visible(poly, s))
            continue;

        assert(getPicClass(level_face[f].tile) == TILE16BPP);
        EZ_polygon(DRAW_MESH | ECDSPD_DISABLE | UCLPIN_ENABLE | COLOR_5 | DRAW_GOURAU, WATERCOLOR, poly, &gtable);
        nmPolys++;
    }
}

struct doorwayCache
{
    sint16 xmin, ymin, xmax, ymax;
    sint32 distance;
    /* xmin=-32000 for walls that are totally rejected */
} doorwayCache[MAXNMWALLS];

#define MAXNMSLAVEPOLYS 1300

/* if tile==-1 then gtable.entry[0]==sector that was just drawn */
struct slaveDrawResult
{
    XyInt poly[4];
    struct gourTable gtable;
    sint16 tile, pad;
};

SectorDrawRecord sectorDraw[MAXNMSECTORS];
SectorDrawRecord* updateList[MAXNMSECTORS];
sint32 updateListSize;
SectorDrawRecord* drawList[MAXNMSECTORS];
sint32 drawListSize;

void drawSector(sint32 sectorNm, MthMatrix* view)
{
    sSectorType* sec;
    sWallType* theWall;
    XyInt poly[4];
    MthXyz wallV[4];
    MthXyz tformed[4];
    MthXyz clipped[4];
    sint32 w, i;

    sec = &(level_sector[sectorNm]);
    sec->flags |= SECFLAG_SEEN;

    assert(camera->pos.x < F(16000) && camera->pos.x > F(-16000) && camera->pos.y < F(16000) && camera->pos.y > F(-16000));

    assert(sectorNm >= 0 && sectorNm < level_nmSectors);
    /* draw walls */
    for (w = sec->firstWall; w <= sec->lastWall; w++)
    {
        theWall = &level_wall[w];
        /* do some validity checks on the wall */
        assert(abs(theWall->normal[0]) <= F(1));
        assert(abs(theWall->normal[1]) <= F(1));
        assert(abs(theWall->normal[2]) <= F(1));

        if (theWall->flags & WALLFLAG_INVISIBLE)
            continue;
        /* back face clipping */
        getVertex(theWall->v[0], wallV + 0);

        if ((f(camera->pos.x - wallV[0].x)) * theWall->normal[0] + (f(camera->pos.y - wallV[0].y)) * theWall->normal[1] + (f(camera->pos.z - wallV[0].z)) * theWall->normal[2] < 0)
            continue;

        /* far plane clipping */
        getVertex(theWall->v[1], wallV + 1);
        getVertex(theWall->v[2], wallV + 2);
        getVertex(theWall->v[3], wallV + 3);

        for (i = 0; i < 4; i++)
            MTH_CoordTrans(view, wallV + i, tformed + i);

#if ENABLEFARCLIP
        if (tformed[0].z > FARCLIP && tformed[1].z > FARCLIP && tformed[2].z > FARCLIP && tformed[3].z > FARCLIP)
            continue;
#endif

        /* near plane clipping */
        if (!(theWall->flags & WALLFLAG_WATERSURFACE))
            if (tformed[0].z < NEARCLIP && tformed[1].z < NEARCLIP && tformed[2].z < NEARCLIP && tformed[3].z < NEARCLIP)
                continue;
        /* clip to near plane */
        clipZ(tformed, clipped, NEARCLIP);

        for (i = 0; i < 4; i++)
            project_point(clipped + i, poly + i);

        if (!clip_visible(poly, sectorDraw + sectorNm))
            continue;

#if WATER
        if (theWall->flags & WALLFLAG_WATERSURFACE)
        {
            drawWaterSurface(theWall, view, sectorDraw + sectorNm);
            continue;
        }
#endif

        if (theWall->flags & WALLFLAG_PARALLAX)
        {
            for (i = 0; i < 4; i++)
            {
                if (poly[i].x < plaxBBxmin)
                    plaxBBxmin = poly[i].x;
                if (poly[i].y < plaxBBymin)
                    plaxBBymin = poly[i].y;
                if (poly[i].x > plaxBBxmax)
                    plaxBBxmax = poly[i].x;
                if (poly[i].y > plaxBBymax)
                    plaxBBymax = poly[i].y;
            }
            continue;
        }

        /* we're going to draw the wall */
        if (level_sector[sectorNm].flags & SECFLAG_WATER)
            wavyIndex = (w & 0x1f) + 1;
        else
            wavyIndex = 0;
        if (theWall->flags & WALLFLAG_PARALLELOGRAM)
            drawRectWall(theWall, tformed, sectorDraw + sectorNm);
        else
            drawWall(theWall, view, sectorDraw + sectorNm);
    }
}

void findDoorways(sint32 sectorNm, MthMatrix* view)
{
    sSectorType* sec;
    sWallType* theWall;
    XyInt poly[4];
    MthXyz wallV[4];
    MthXyz tformed[4];
    MthXyz clipped[4];
    fix32 wallDist;
    sint32 w, i, polyGood;
    SectorDrawRecord* sr;
    SectorDrawRecord* next;
    sint32 xmin, ymin, xmax, ymax, expanded;
    sint32 cacheValid;

    sec = &(level_sector[sectorNm]);
    sr = sectorDraw + sectorNm;
    cacheValid = sr->flags & SDFLAG_CACHEVALID;
    sr->flags |= SDFLAG_CACHEVALID;

    for (w = sec->firstWall; w <= sec->lastWall; w++)
    {
        theWall = &level_wall[w];
        if (theWall->nextSector == -1)
            /* doorways are sorted to be first in the list */
            return;
        if (theWall->flags & WALLFLAG_BLOCKSSIGHT)
            continue;
        if (!cacheValid)
        {
            doorwayCache[w].xmin = -32000;

            /* back face clipping */
            getVertex(theWall->v[0], wallV + 0);

            wallDist = (f(camera->pos.x - wallV[0].x)) * theWall->normal[0] + (f(camera->pos.y - wallV[0].y)) * theWall->normal[1] + (f(camera->pos.z - wallV[0].z)) * theWall->normal[2];
            if (sectorNm != camera->s /* we may apear to be behind walls of the
                                         sector we are in, but we really aren't */
                && wallDist <= 0) /* increase this number to increase
                                             protection from draw loops */
                continue;
            if (theWall->nextSector == camera->s)
                /* we know that we can not see thru a wall into the sector the
                   camera is in */
                continue;

            getVertex(theWall->v[1], wallV + 1);
            getVertex(theWall->v[2], wallV + 2);
            getVertex(theWall->v[3], wallV + 3);

            /* clip out doorways that have 0 height, so can't see under doors */
            if (theWall->normal[1] == 0 && wallV[0].y == wallV[3].y && wallV[0].y == wallV[2].y && wallV[0].y == wallV[1].y)
                continue;

            /* far plane clipping */
            for (i = 0; i < 4; i++)
                MTH_CoordTrans(view, wallV + i, tformed + i);

#if ENABLEFARCLIP
            if (tformed[0].z > FARCLIP && tformed[1].z > FARCLIP && tformed[2].z > FARCLIP && tformed[3].z > FARCLIP)
                continue;
#endif

            /* near plane clipping */
            if (tformed[0].z < SECTORBNDRYNEARCLIP && tformed[1].z < SECTORBNDRYNEARCLIP && tformed[2].z < SECTORBNDRYNEARCLIP && tformed[3].z < SECTORBNDRYNEARCLIP)
                continue;

            if (wallDist < F(48) && (tformed[0].z < NEARCLIP || tformed[1].z < NEARCLIP || tformed[2].z < NEARCLIP || tformed[3].z < NEARCLIP))
                polyGood = 0;
            else
            {
                /* clip to near plane */
                clipZ(tformed, clipped, NEARCLIP);
                for (i = 0; i < 4; i++)
                    project_point(clipped + i, poly + i);

                polyGood = 1;
                if (!clip_visible(poly, sectorDraw + camera->s /*so cache will be good */))
                    continue;
            }
            /* find bounding box for opening */
            if (!polyGood)
            {
                xmin = XMIN;
                ymin = YMIN;
                xmax = XMAX;
                ymax = YMAX;
            }
            else
            {
#if 0
	     if (theWall->flags & WALLFLAG_BLOCKED)
		{addDebugLine(poly[0].x,poly[0].y,poly[1].x,poly[1].y);
		 addDebugLine(poly[1].x,poly[1].y,poly[2].x,poly[2].y);
		 addDebugLine(poly[2].x,poly[2].y,poly[3].x,poly[3].y);
		 addDebugLine(poly[3].x,poly[3].y,poly[0].x,poly[0].y);
		}
#endif
                xmin = 1000;
                ymin = 1000;
                xmax = -1000;
                ymax = -1000;
                for (i = 0; i < 4; i++)
                {
                    if (poly[i].x < xmin)
                        xmin = poly[i].x;
                    if (poly[i].x > xmax)
                        xmax = poly[i].x;
                    if (poly[i].y < ymin)
                        ymin = poly[i].y;
                    if (poly[i].y > ymax)
                        ymax = poly[i].y;
                }
            }
            doorwayCache[w].xmin = xmin;
            doorwayCache[w].ymin = ymin;
            doorwayCache[w].xmax = xmax;
            doorwayCache[w].ymax = ymax;

            /* doorwayCache[w].distance=
               (tformed[0].z+tformed[1].z+tformed[2].z+tformed[3].z)>>2; */
            /* that distance approx wasn't good enough for some cases,
               maybe this one will be better.  Maybe we could just use two of the
               verticies instead of all 4 */
#if 0
	 (sint32 cx=0,cy=0,cz=0;
	  for (i=0;i<4;i++)
	     {cx+=tformed[i].x;
	      cy+=tformed[i].y;
	      if (tformed[i].z>0)
		 cz+=tformed[i].z;
	     }
	  doorwayCache[w].distance=
	     approxDist(cx,cy,(cz<<2));
	 }
#else
            /* that one still had some problems, try this one: */
            {
                sint32 minx = INT_MAX, xposCnt = 0;
                sint32 miny = INT_MAX, yposCnt = 0;
                sint32 minz = INT_MAX, zposCnt = 0;
                sint32 j;
                for (i = 0; i < 4; i++)
                {
                    if (tformed[i].x < 0)
                        j = -tformed[i].x;
                    else
                    {
                        xposCnt++;
                        j = tformed[i].x;
                    }
                    if (j < minx)
                        minx = j;

                    if (tformed[i].y < 0)
                        j = -tformed[i].y;
                    else
                    {
                        yposCnt++;
                        j = tformed[i].y;
                    }
                    if (j < miny)
                        miny = j;

                    if (tformed[i].z < 0)
                        j = -tformed[i].z;
                    else
                    {
                        zposCnt++;
                        j = tformed[i].z;
                    }
                    if (j < minz)
                        minz = j;
                }
                if (xposCnt != 0 && xposCnt != 4)
                    minx = 0;
                if (yposCnt != 0 && yposCnt != 4)
                    miny = 0;
                if (zposCnt != 0 && zposCnt != 4)
                    minz = 0;
                doorwayCache[w].distance = approxDist(minx, miny, minz);
            }
#endif
        }
        else
        { /* cache is valid */
            xmin = doorwayCache[w].xmin;
            if (xmin == -32000)
                continue;
            xmax = doorwayCache[w].xmax;
            ymin = doorwayCache[w].ymin;
            ymax = doorwayCache[w].ymax;
        }

        next = sectorDraw + theWall->nextSector;

        /* clip to our bounding box */
        if (xmin < sr->xmin)
            xmin = sr->xmin;
        if (ymin < sr->ymin)
            ymin = sr->ymin;
        if (xmax > sr->xmax)
            xmax = sr->xmax;
        if (ymax > sr->ymax)
            ymax = sr->ymax;
        if (xmax <= xmin || ymax <= ymin)
            continue;
        /* merge current bounding box with new bbox */
        if (!(next->flags & SDFLAG_BBVALID))
        {
            next->xmin = xmin;
            next->ymin = ymin;
            next->xmax = xmax;
            next->ymax = ymax;
            next->flags |= (SDFLAG_NEEDTOPROCESS | SDFLAG_BBVALID);
            next->distance = doorwayCache[w].distance;
            updateList[updateListSize++] = next;
        }
        else
        { /* new code here */
            if (doorwayCache[w].distance < next->distance)
                next->distance = doorwayCache[w].distance;
            /* end of new scary code */
            expanded = 0;
            if (xmin < next->xmin)
            {
                next->xmin = xmin;
                expanded = 1;
            }
            if (ymin < next->ymin)
            {
                next->ymin = ymin;
                expanded = 1;
            }
            if (xmax > next->xmax)
            {
                next->xmax = xmax;
                expanded = 1;
            }
            if (ymax > next->ymax)
            {
                next->ymax = ymax;
                expanded = 1;
            }
            if (expanded)
                next->flags |= SDFLAG_NEEDTOPROCESS;
        }
    }
}

void buildTree(void)
{
    sint32 update, s, w, adjoin;
    /* each wall of each sector in the update list needs to be marked if
       the adjoining sector is also in the draw list */
    for (update = 0; update < updateListSize; update++)
    {
        s = updateList[update] - sectorDraw;
        for (w = level_sector[s].firstWall; w <= level_sector[s].lastWall; w++)
        {
            adjoin = level_wall[w].nextSector;
            if (adjoin == -1)
                break;
            if (level_wall[w].flags & WALLFLAG_BLOCKSSIGHT)
                continue;
            if (!(sectorDraw[adjoin].flags & SDFLAG_BBVALID))
                continue;
            /* each wall is two sided, so we may only do the ancestor->child
               relationships */

            assert(sectorDraw[adjoin].flags & SDFLAG_CACHEVALID);
            if (doorwayCache[w].xmin == -32000)
                /* this means wall normal points away from eye */
                continue;

            /* here we know that this wall represents an arrow from s to
               adjoin; s is adjoin's child */

            assert(sectorDraw[adjoin].nmAncestors < MAXFANIN);
            sectorDraw[adjoin].ancestor[(sint32)sectorDraw[adjoin].nmAncestors++] = s;
            sectorDraw[s].nmChildren++;
        }
    }
}

static void sortLeafList(SectorDrawRecord** leafList, sint32 leafListSize)
{
    sint32 i, j;
    sSectorType *s1, *s2;
    SectorDrawRecord* save;

    /* sort by distance */
    for (i = 1; i < leafListSize; i++)
    {
        save = leafList[i];
        for (j = i; j > 0; j--)
            if (leafList[j - 1]->distance < save->distance)
                leafList[j] = leafList[j - 1];
            else
                break;
        if (j != i)
            leafList[j] = save;
    }

    /* sort by cut plane */
    for (i = 0; i < leafListSize; i++)
    {
        s1 = level_sector + (leafList[i] - sectorDraw);
        if (!(s1->flags & SECFLAG_CUTSORT))
            continue;
        /* look thru rest of list for sectors we are on wrong side of */
        for (j = leafListSize - 1; j > i; j--)
        {
            s2 = level_sector + (leafList[j] - sectorDraw);
            if (!(s2->flags & SECFLAG_CUTSORT))
                continue;
            if (s1->cutChannel != s2->cutChannel)
                continue;
            { /* sort via cut planes */
                sint32 plane = level_cutPlane[s1->cutIndex * MAXCUTSECTORS + s2->cutIndex];
                sWallType* cutWall;
                MthXyz testV, p;
                /* if (debugFlag)
                   dPrint(" cut sorting %d & %d\n",
                          s1-level_sector,s2-level_sector); */
                assert(plane != 99);
                if (plane & 0x80)
                    cutWall = level_wall + (s2->firstWall + (plane & 0x7f));
                else
                    cutWall = level_wall + (s1->firstWall + (plane & 0x7f));
                /* if (debugFlag)
                   {dPrint(" cut wall %d (%d,%d,%d)\n",
                           cutWall-level_wall,
                           cutWall->normal[0],
                           cutWall->normal[1],
                           cutWall->normal[2]);
                   } */
                getVertex(cutWall->v[0], &testV);
                p.x = camera->pos.x - testV.x;
                p.y = camera->pos.y - testV.y;
                p.z = camera->pos.z - testV.z;
                if (((plane & 0x80) && MTH_Product((fix32*)&p, (fix32*)cutWall->normal) < 0) || (!(plane & 0x80) && MTH_Product((fix32*)&p, (fix32*)cutWall->normal) > 0))
                { /* move s1 so that it is after s2 */
                    sint32 k;
                    save = leafList[i];
                    for (k = i; k < j; k++)
                        leafList[k] = leafList[k + 1];
                    leafList[k] = save;
                    break;
                }
            }
        }
    }
    /* if (debugFlag)
        {for (i=0;i<leafListSize;i++)
            dPrint("%d ",leafList[i]-sectorDraw);
         dPrint("\n");
        } */
}

void drawWalls(MthMatrix* view)
{
    sint32 i;
    XyInt parms[2];
    sint32 done;
    sint32 leafListSize;
    sint32 leafToDraw;
    sint32 lastWallCmd;
    SectorDrawRecord *leafList[MAXNMSECTORS], *sdr;

    plaxBBxmin = 160;
    plaxBBymin = 120;
    plaxBBxmax = -160;
    plaxBBymax = -120;

    for (i = 0; i < nmLights; i++)
        MTH_CoordTrans(view, &(lightSource[i]->pos), tLightPos + i);

    pushProfile("Find Visible");

    nmPolys = 0;
    autoTarget = NULL;
    bestAutoAimRating = INT_MAX;
    for (i = 0; i < level_nmSectors; i++)
        sectorDraw[i].flags = 0;
    assert(camera->s >= 0 && camera->s < level_nmSectors);

    sectorDraw[camera->s].xmin = XMIN;
    sectorDraw[camera->s].ymin = YMIN;
    sectorDraw[camera->s].xmax = XMAX;
    sectorDraw[camera->s].ymax = YMAX;
    sectorDraw[camera->s].flags |= SDFLAG_BBVALID;
    updateListSize = 1;
    updateList[0] = sectorDraw + camera->s;
    pushProfile("Find Doorways");
    findDoorways(camera->s, view);
    do
    {
        done = 1;
        for (i = 0; i < updateListSize; i++)
        {
            sint32 s = updateList[i] - sectorDraw;
            if (sectorDraw[s].flags & SDFLAG_NEEDTOPROCESS)
            {
                sectorDraw[s].flags &= ~SDFLAG_NEEDTOPROCESS;
                findDoorways(s, view);
                done = 0;
            }
        }
    } while (!done);
    popProfile();

#if 0
 for (i=0;i<updateListSize;i++)
    {addDebugLine(updateList[i]->xmin,updateList[i]->ymin,
		  updateList[i]->xmax,updateList[i]->ymin);
     addDebugLine(updateList[i]->xmax,updateList[i]->ymin,
		  updateList[i]->xmax,updateList[i]->ymax);
     addDebugLine(updateList[i]->xmax,updateList[i]->ymax,
		  updateList[i]->xmin,updateList[i]->ymax);
     addDebugLine(updateList[i]->xmin,updateList[i]->ymax,
		  updateList[i]->xmin,updateList[i]->ymin);
    }
#endif

    /* build tree */
    for (i = 0; i < updateListSize; i++)
    {
        updateList[i]->nmAncestors = 0;
        updateList[i]->nmChildren = 0;
    }
    buildTree();

#if 0
 /* I think I fixed this in findDoorways */

 /* if currentSector ended up with any ancestors (which can only
    happen when we are standing right on a sector boundry)
    then remove them */
 for (i=0;i<sectorDraw[camera->s].nmAncestors;i++)
    sectorDraw[sectorDraw[camera->s].ancestor[i]].nmChildren--;
 sectorDraw[camera->s].nmAncestors=0;
#endif

#if 0
#ifndef NDEBUG
 if (debugFlag)
    {for (i=0;i<updateListSize;i++)
	{dPrint("s:%d dist:%d\n",updateList[i]-sectorDraw,
		updateList[i]->distance);
	 dPrint(" #c=%d ",updateList[i]->nmChildren);
	 for (j=0;j<updateList[i]->nmAncestors;j++)
	    dPrint("%d ",updateList[i]->ancestor[j]);
	 dPrint("\n");
	}
     debugFlag=0;
    }
#endif
#endif

#ifndef NDEBUG
    if (updateListSize > 1)
        for (i = 0; i < updateListSize; i++)
            assert(updateList[i]->nmChildren > 0 || updateList[i]->nmAncestors > 0);
    for (i = 0; i < updateListSize; i++)
        assert(updateList[i] == &sectorDraw[camera->s] || updateList[i]->nmAncestors > 0);
#endif

    leafListSize = 0;
    /* add leaves to leaf list */
    for (i = 0; i < updateListSize; i++)
        if (updateList[i]->nmChildren == 0)
            leafList[leafListSize++] = updateList[i];

    /* sort list */
    sortLeafList(leafList, leafListSize);

    drawListSize = 0;
    /* itterate: choose leaf to draw, add this leaf to draw list and
       remove it from the leaf list, add all the newly created leaves to
       the leaf list */
    while (drawListSize < updateListSize)
    {
        SectorDrawRecord* breakLeaf = NULL;

        leafToDraw = 0;

        if (leafListSize <= 0)
        { /* there are still sectors we haven't drawn, we must have a
             circular dependancy, try to break it */
            /* this doesn't happen normally */
            /* ... find the sector in the updateList which has the minimum
               nonzero # of children */
            sint32 bestNmChildren;
            sint32 i;
            bestNmChildren = 1000;
            for (i = 0; i < updateListSize; i++)
                if (updateList[i]->nmChildren > 0 && updateList[i]->nmChildren < bestNmChildren)
                {
                    bestNmChildren = updateList[i]->nmChildren;
                    breakLeaf = updateList[i];
                }
            assert(bestNmChildren != 1000);
            /* ... remove all breakLeaf's children */
            /* NOTE: this breaks the tree structure somewhat, as breakLeaf is
               still referenced by ancestor pointers in other sectors. */
            breakLeaf->nmChildren = 0;
            leafList[leafListSize++] = breakLeaf;
            assert(leafListSize > 0);
        }
        assert(leafListSize > 0);

        /* add leafToDraw to drawList */
        drawList[drawListSize++] = leafList[leafToDraw];
        /* remove leafToDraw from leafList */
        for (i = leafToDraw + 1; i < leafListSize; i++)
            leafList[i - 1] = leafList[i];
        leafListSize--;
        /* pluck leafToDraw from tree and add new leaves to leaf list */
        for (i = 0; i < drawList[drawListSize - 1]->nmAncestors; i++)
        {
            sdr = &(sectorDraw[drawList[drawListSize - 1]->ancestor[i]]);
            if ((--sdr->nmChildren) == 0)
                /* add to leaf list */
                leafList[leafListSize++] = sdr;
        }

        /* sort list */
        sortLeafList(leafList, leafListSize);
    }
    assert(leafListSize == 0); /* crashes here for some damn reason */
    /* temp bandaid */
    updateListSize = drawListSize;
    for (i = 0; i < updateListSize; i++)
        updateList[i] = drawList[updateListSize - i - 1];

    popProfile();

    for (i = updateListSize - 1; i >= 0; i--)
    {
        parms[0].x = updateList[i]->xmin + 160;
        parms[0].y = updateList[i]->ymin + 120;
        parms[1].x = updateList[i]->xmax + 160;
        parms[1].y = updateList[i]->ymax + 120;
#if RECTCLIP
        EZ_userClip(parms);
#endif
        drawSector(updateList[i] - sectorDraw, view);
        drawSprites(&(camera->pos), view, updateList[i] - sectorDraw);
    }

    lastWallCmd = EZ_getNextCmdNm() - 1;
    /* draw sprites in slave rendered sectors */
    for (; i >= 0; i--)
    {
        sint32 last = EZ_getNextCmdNm();
        drawSprites(&(camera->pos), view, updateList[i] - sectorDraw);
        if (EZ_getNextCmdNm() != last)
        {
            EZ_linkCommand(EZ_getNextCmdNm() - 1, JUMP_RETURN, 0);
            updateList[i]->spriteCommandStart = last;
        }
        else
            updateList[i]->spriteCommandStart = 0;
    }
    EZ_linkCommand(lastWallCmd, JUMP_ASSIGN, EZ_getNextCmdNm());
}

void drawWallsFinish(void)
{
#ifndef NDEBUG
    drawDebugLines();
#endif
    updateLights();
    /* merge slave and master plax bbs */
    if (plaxBBxmin < XMIN)
        plaxBBxmin = XMIN;
    if (plaxBBymin < YMIN)
        plaxBBymin = YMIN;
    if (plaxBBxmax > XMAX)
        plaxBBxmax = XMAX;
    if (plaxBBymax > YMAX)
        plaxBBymax = YMAX;
}

#if 0
static sint16 internalHead[MAXNMSECTORS];
static sint16 internalList[MAXNMSECTORS];
static init=0;
void initInternalList(void)
(sint32 s,w,i;
 sint32 list,nm;
 sint8 already[MAXNMSECTORS];
 list=0;
 for (s=0;s<level_nmSectors;s++)
    {internalHead[s]=list;
     nm=0;
     for (i=0;i<MAXNMSECTORS;i++)
	already[i]=0;
     for (w=level_sector[s].firstWall;
	  w<=level_sector[s].lastWall;
	  w++)
	if ((level_wall[w].flags & WALLFLAG_MOBILE) &&
	    level_wall[w].nextSector!=-1 &&
	    !already[level_wall[w].nextSector])
	   {internalList[list++]=level_wall[w].nextSector;
	    already[level_wall[w].nextSector]=1;
	    nm++;
	   }
     if (nm==0)
	internalHead[s]=-1;
     else
	{internalList[list++]=-1;
	}
     assert(list<MAXNMSECTORS);
    }
}
#endif

/* returns 1 if clipped out */
sint32 frustumClip(fix32* p1, fix32* p2, sint32 dx, sint32 dy, sint32 dz, sint32 neg)
{
    fix32 t, denom;
    fix32 delX, delY, delZ;
    fix32* badPoint;
    /* see if p2 is on wrong side of plane */
    delX = p2[dx] - p1[dx];
    delY = p2[dy] - p1[dy];
    badPoint = NULL;
    if (!neg)
    {
        if (p2[dx] > p2[dy])
        {
            badPoint = p2;
        }
        if (p1[dx] > p1[dy])
        {
            if (badPoint)
                return 1;
            badPoint = p1;
        }
        if (!badPoint)
            return 0;

        denom = delX - delY;
        if (abs(denom) < 10)
            return 0;
        t = MTH_Div(p1[dy] - p1[dx], denom);
    }
    else
    {
        if (p2[dx] < -p2[dy])
        {
            badPoint = p2;
        }
        if (p1[dx] < -p1[dy])
        {
            if (badPoint)
                return 1;
            badPoint = p1;
        }
        if (!badPoint)
            return 0;
        denom = delX + delY;
        if (abs(denom) < 10)
            return 0;
        t = MTH_Div(-p1[dy] - p1[dx], denom);
    }
    delZ = p2[dz] - p1[dz];
    badPoint[dx] = p1[dx] + MTH_Mul(t, delX);
    if (neg)
        badPoint[dy] = -badPoint[dx];
    else
        badPoint[dy] = badPoint[dx];
    badPoint[dz] = p1[dz] + MTH_Mul(t, delZ);
    return 0;
}

void drawSprites(MthXyz* playerPos, MthMatrix* view, sint32 sector)
{
    Sprite* o;
    Sprite* drawList[100];
    sint32 nmDraw, draw;
    sint32 chunk, light, x, y, i, j;
    sint32 flip;
    sint32 frame;
    fix32 width64, scale;
    MthXyz tformed, feetPos;
    XyInt pos[4];
    XyInt feetScreenPos;

#if 0
 if (!init)
    {initInternalList();
     init=1;
    }
#endif

    nmDraw = 0;
    assert(sector >= 0 && sector < level_nmSectors);

    assert(camera->sequence == -1);
    for (o = sectorSpriteList[sector]; nmDraw < 100 && o; o = o->next)
        if (o->sequence != -1 && !(o->flags & SPRITEFLAG_INVISIBLE))
            drawList[nmDraw++] = o;

#if 0
 if (internalHead[sector]!=-1)
    {assert(0);
     for (i=internalHead[sector];internalList[i]!=-1;i++)
	{for (o=sectorSpriteList[internalList[i]];o;o=o->next)
	    drawList[nmDraw++]=o;
	}
    }
#endif

    assert(nmDraw < 100);

    if (nmDraw == 0)
        return;

    /* sort */
    for (i = 1; i < nmDraw; i++)
        for (j = i; j > 0; j--)
        {
            sint32 d1, d2;
            Sprite* swap;
            d1 = f(abs(drawList[j - 1]->pos.x - playerPos->x)) + f(abs(drawList[j - 1]->pos.y - playerPos->y)) + f(abs(drawList[j - 1]->pos.z - playerPos->z));
            d2 = f(abs(drawList[j]->pos.x - playerPos->x)) + f(abs(drawList[j]->pos.y - playerPos->y)) + f(abs(drawList[j]->pos.z - playerPos->z));
            if (d1 < d2)
            {
                swap = drawList[j];
                drawList[j] = drawList[j - 1];
                drawList[j - 1] = swap;
            }
            else
                break;
        }

        /* assert(nmDraw<=1);*/

#if RECTCLIP
    pos[0].x = XMIN + 160;
    pos[0].y = YMIN + 120;
    pos[1].x = XMAX + 160;
    pos[1].y = YMAX + 120;
    EZ_userClip(pos);
#endif

    for (draw = 0; draw < nmDraw; draw++)
    {
        o = drawList[draw];
        assert(o->sequence >= 0);
#if 0
     if (o->flags & OBJECTFLAG_PUSHBLOCK)
	{/* draw push block */
	 sint32 w;
	 sPBType *pb=level_pushBlock+o->sequence;
	 sWallType *theWall;
	 for (w=pb->startWall;w<=pb->endWall;w++)
	    {theWall=level_wall+level_PBWall[w];
	     renderWall(theWall,view,sector);
	    }
	 continue;
	}
#endif
        if (o->flags & SPRITEFLAG_LINE)
        { /* draw line */
            MthXyz p1, p2;
            sint32 width;
            MTH_CoordTrans(view, &o->pos, &p1);
            feetPos.x = o->angle;
            feetPos.y = o->scale;
            feetPos.z = o->frame;
            MTH_CoordTrans(view, &feetPos, &p2);
            /* p1 & p2 hold view space endpoints of line */
            if (o->flags & SPRITEFLAG_THINLINE)
            {
                if (frustumClip((fix32*)&p1, (fix32*)&p2, 0, 2, 1, 1) || frustumClip((fix32*)&p1, (fix32*)&p2, 0, 2, 1, 0) || frustumClip((fix32*)&p1, (fix32*)&p2, 1, 2, 0, 0) || frustumClip((fix32*)&p1, (fix32*)&p2, 1, 2, 0, 1))
                    continue;
                project_point(&p1, pos);
                project_point(&p2, pos + 1);

                EZ_line(UCLPIN_ENABLE | COMPO_TRANS | ECDSPD_DISABLE | COLOR_5,
                    RGB(abs(laserColor - 31), 15, 31 - abs(laserColor - 31))
                    /*o->color*/,
                    pos, NULL);
                continue;
            }
            if (p1.z < F(32) || p2.z < F(32))
                continue;
            project_point(&p1, pos);
            project_point(&p2, pos + 1);
            width = f(MTH_Div(2 * F(FOCALDIST), p2.z));
            pos[2].x = pos[1].x;
            pos[2].y = pos[1].y - width;
            pos[3].x = pos[0].x;
            pos[3].y = pos[0].y - width;
            EZ_polygon(UCLPIN_ENABLE | ECDSPD_DISABLE | COLOR_5, o->color, pos, NULL);
            continue;
        }
        if (o->owner)
            signalObject(o->owner, SIGNAL_VIEW, 0, 0);
        feetPos.x = o->pos.x;
        feetPos.y = o->pos.y - o->radius;
        feetPos.z = o->pos.z;
        MTH_CoordTrans(view, &feetPos, &tformed);
        if (tformed.z < F(32))
            continue;
        /* if (tformed.z>FARCLIP)
           continue; */

        /* light=tformed.z>>((16+FARCLIP2)-3);
        if (light>NMOBJECTPALLETES-1) light=NMOBJECTPALLETES-1; */
        light = 0;

        if (o->flags & SPRITEFLAG_FLASH)
        {
            light = NMOBJECTPALLETES;
            o->flags &= ~SPRITEFLAG_FLASH;
        }
        /* tformed is center of sprite */
        project_point(&tformed, &feetScreenPos);
        scale = o->scale;
        scale = MTH_Div(scale * FOCALDIST, tformed.z);
        if (o->flags & SPRITEFLAG_NOSCALE)
            scale = 65536;
        /* if (o->flags & SPRITEFLAG_32x32)
           width64=scale>>11;
           else */
        width64 = scale >> 10;

        if (o->owner && o->owner->class == CLASS_MONSTER && feetScreenPos.x < 20 && feetScreenPos.x > -20)
        { /* object is an autoaiming candidate */
            sint32 rate;
            rate = f(tformed.z) + abs(feetScreenPos.y << 4) + abs(feetScreenPos.x << 4);
            if (rate < bestAutoAimRating)
            {
                bestAutoAimRating = rate;
                autoTarget = o;
            }
        }
        /* draw shadow */
        if (!(o->flags & SPRITEFLAG_NOSHADOW))
        {
            fix32 shadowHeight;
            fix32 shadowWidth;
            fix32 shadowScale;
            XyInt shadowScreenPos;
            MthXyz shadowPos;
            shadowPos.x = feetPos.x;
            shadowPos.z = feetPos.z;
            shadowPos.y = feetPos.y - findFloorDistance(o->s, &feetPos);
            shadowScale = (F(128) - abs(shadowPos.y - feetPos.y)) >> 7;
            if (shadowScale > 0)
            {
                MTH_CoordTrans(view, &shadowPos, &tformed);
                if (tformed.z > F(32) && tformed.z < FARCLIP)
                {
                    project_point(&tformed, &shadowScreenPos);
                    shadowScale = MTH_Mul(shadowScale, scale);
                    shadowWidth = 48 * shadowScale;
                    shadowHeight = 48 * shadowScale;

                    shadowHeight = MTH_Mul(shadowHeight, MTH_Div(abs(shadowPos.y - playerPos->y), tformed.z));
                    pos[0].x = shadowScreenPos.x;
                    pos[0].y = shadowScreenPos.y;
                    pos[1].x = f(shadowWidth);
                    pos[1].y = f(shadowHeight);
                    assert(getPicClass(0) != TILEVDP);
                    EZ_scaleSpr(ZOOM_MM, COLOR_4 | COMPO_SHADOW, 0, mapPic(0), pos, NULL);
                }
            }
        }
        if (o->flags & SPRITEFLAG_FOOTCLIP)
        {
            if (feetScreenPos.y < sectorDraw[sector].ymax)
            {
                pos[0].x = sectorDraw[sector].xmin + 160;
                pos[0].y = sectorDraw[sector].ymin + 120;
                pos[1].x = sectorDraw[sector].xmax + 160;
                pos[1].y = feetScreenPos.y + 120;
                EZ_userClip(pos);
            }
        }

        /* draw sprite */
        frame = o->frame + level_sequence[o->sequence];
        for (chunk = level_frame[frame].chunkIndex; chunk < level_frame[frame + 1].chunkIndex; chunk++)
        {
            x = level_chunk[chunk].chunkx;
            y = level_chunk[chunk].chunky;
            pos[0].x = feetScreenPos.x + f(scale * x);
            pos[0].y = feetScreenPos.y + f(scale * y);
            pos[1].x = width64;
            pos[1].y = width64;
            flip = 0;
            if (level_chunk[chunk].flags & 1)
                flip |= DIR_LRREV;
            if (level_chunk[chunk].flags & 2)
                flip |= DIR_TBREV;
#if 0
	 {XyInt p[4];
	  sint32 r=f(MTH_Mul(o->radius,scale));
	  p[0].x=feetScreenPos.x-r;
	  p[0].y=feetScreenPos.y-r;
	  p[1].x=feetScreenPos.x+r;
	  p[1].y=feetScreenPos.y-r;
	  p[2].x=feetScreenPos.x+r;
	  p[2].y=feetScreenPos.y+r;
	  p[3].x=feetScreenPos.x-r;
	  p[3].y=feetScreenPos.y+r;
	  EZ_polygon(ECDSPD_DISABLE|COMPO_REP|COLOR_5,0xffff,p,NULL);
	 }
#endif

            assert(getPicClass(level_chunk[chunk].tile != TILEVDP));
            {
                sint32 pic = mapPic(level_chunk[chunk].tile);

                i = getPicClass(level_chunk[chunk].tile);
                if (i != TILE8BPP && i != TILESMALL8BPP)
                {
                    struct gourTable gtable;
                    if (i == TILESMALL16BPP)
                    {
                        pos[1].x >>= 1;
                        pos[1].y >>= 1;
                    }
                    if (o->flags & SPRITEFLAG_COLORED)
                        gtable.entry[0] = o->color;
                    else
                    {
                        if (light == NMOBJECTPALLETES)
                            gtable.entry[0] = RGB(31, 31, 31);
                        else
                            gtable.entry[0] = greyTable[16 - light * 2];
                    }
                    gtable.entry[1] = gtable.entry[0];
                    gtable.entry[2] = gtable.entry[0];
                    gtable.entry[3] = gtable.entry[0];
                    EZ_scaleSpr(ZOOM_TL | flip, UCLPIN_ENABLE | COLOR_5 | HSS_ENABLE | ECD_DISABLE | DRAW_GOURAU, 0, pic, pos, &gtable);
                }
                else
                {
                    if (i == TILESMALL8BPP)
                    {
                        pos[1].x >>= 1;
                        pos[1].y >>= 1;
                    }
                    EZ_scaleSpr(ZOOM_TL | flip, UCLPIN_ENABLE | COLOR_4 | HSS_ENABLE | ECD_DISABLE, light << 8, pic, pos, NULL);
                }
            }
        }
        /* done drawing sprite */
        if (o->flags & SPRITEFLAG_FOOTCLIP)
        { /* pos[0].x=sectorDraw[sector].xmin+160;
             pos[0].y=sectorDraw[sector].ymin+120;
             pos[1].x=sectorDraw[sector].xmax+160;
             pos[1].y=sectorDraw[sector].ymax+120; */
            pos[0].x = XMIN + 160;
            pos[0].y = YMIN + 120;
            pos[1].x = XMAX + 160;
            pos[1].y = YMAX + 120;
            EZ_userClip(pos);
        }
    }
}

void initWallRenderer(void)
{
    lightInit();
}
