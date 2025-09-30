#include <machine.h>
#include <sega_spr.h>
#include <sega_scl.h>
#include <sega_int.h>
#include <sega_mth.h>
#include <sega_sys.h>
#include <sega_dbg.h>
#include <sega_per.h>

#include "util.h"
#include "spr.h"
#include "level.h"
#include "sprite.h"
#include "sruins.h"
#include "print.h"
#include "bigmap.h"
#include "gamestat.h"
#include "local.h"

#define MINX (-160)
#define MAXX (160)
#define MINY (-110)
#define MAXY (90)

sint8 mapColor[MAXNMWALLS];

static fix32 mapScale;
static sint32 gotFullMap;

void mapScaleUp(void)
{
    mapScale = MTH_Mul(mapScale, 60000);
    if (mapScale < 2000)
        mapScale = 2000;
}

void mapScaleDn(void)
{
    mapScale = MTH_Mul(mapScale, 70000);
    if (mapScale > 20000)
        mapScale = 20000;
}

void revealMap(void)
{
    gotFullMap = 1;
}

void initMap(void)
{
    sint32 s, w;
    MthXyz t;
    mapScale = F(1) / 8;
    gotFullMap = 0;
    for (s = 0; s < level_nmSectors; s++)
    {
        for (w = level_sector[s].firstWall; w <= level_sector[s].lastWall; w++)
        {
            mapColor[w] = 0;
            if (level_wall[w].normal[1] != 0)
                continue;
            if (/*level_wall[w].nextSector==-1*/
                level_wall[w].flags & WALLFLAG_BLOCKED)
            { /* only draw wall if goes from floor to ceiling */
                if (findCeilDistance(s, getVertex(level_wall[w].v[0], &t)) < F(1) && findFloorDistance(s, getVertex(level_wall[w].v[3], &t)) < F(1) && findCeilDistance(s, getVertex(level_wall[w].v[1], &t)) < F(1) && findFloorDistance(s, getVertex(level_wall[w].v[2], &t)) < F(1))
                    mapColor[w] = 1;
                continue;
            }
            else
            { /* only draw wall if floor is different heights on both sides */
                fix32 minDiff, f;
                minDiff = abs(findFloorDistance(s, getVertex(level_wall[w].v[3], &t)) - findFloorDistance(level_wall[w].nextSector, getVertex(level_wall[w].v[3], &t)));
                f = abs(findFloorDistance(s, getVertex(level_wall[w].v[2], &t)) - findFloorDistance(level_wall[w].nextSector, getVertex(level_wall[w].v[2], &t)));
                if (f > minDiff)
                    minDiff = f;

                if (minDiff > F(1))
                {
                    mapColor[w] = 2;
                    if (minDiff < F(32))
                        mapColor[w] = 3;
                }
                continue;
            }
        }
    }
}

#ifndef NDEBUG
#define MAXMARKS 5
static sint32 markX[MAXMARKS], markY[MAXMARKS];
static sint32 nmMarks = 0;
void setMark(fix32 x, fix32 y)
{
    if (nmMarks == MAXMARKS)
        return;
    markX[nmMarks] = x;
    markY[nmMarks] = y;
    nmMarks++;
}
#else
void setMark(fix32 x, fix32 y)
{
    return;
}
#endif

static uint16 colors[4] = { 0, 0x8000 | 31 << 10 | 31 << 5 | 31, 0x8000 | 25 << 10 | 25 << 5 | 25, 0x8000 | 18 << 10 | 18 << 5 | 18 };

#define ARROWR 5

void drawMap(sint32 cx, sint32 cy, sint32 cz, sint32 yaw, sint32 currentSector)
{
#ifndef NDEBUG
    sint32 i;
#endif
    sint32 w, s, color, transp;
    sint16 currentHeight;
    XyInt mapLine[2];
    MthXyz north, east;
    MthXyz p1, p2, wallP;
    north.x = MTH_Mul(MTH_Cos(yaw), mapScale);
    north.y = MTH_Mul(MTH_Sin(yaw), mapScale);
    east.x = north.y;
    east.y = -north.x;

    {
        mapLine[0].x = MINX + 160;
        mapLine[0].y = MINY + 120;
        mapLine[1].x = MAXX + 160;
        mapLine[1].y = MAXY + 120;
        EZ_userClip(mapLine);
    }
#ifndef NDEBUG
    for (i = 0; i < nmMarks; i++)
    {
        sint32 x, y, tx, ty;
        XyInt mark[4];
        x = f(markX[i] - cx);
        y = f(markY[i] - cy);
        tx = f(x * north.x + y * north.y);
        ty = f(x * east.x + y * east.y);
        mark[0].x = tx - 5;
        mark[0].y = ty - 5;
        mark[1].x = tx + 5;
        mark[1].y = ty - 5;
        mark[2].x = tx + 5;
        mark[2].y = ty + 5;
        mark[3].x = tx - 5;
        mark[3].y = ty + 5;
        EZ_polygon(ECDSPD_DISABLE | COMPO_REP | COLOR_5, 0x8ff0, mark, NULL);
    }
    nmMarks = 0;
#endif

    currentHeight = level_sector[currentSector].floorLevel;
    for (s = 0; s < level_nmSectors; s++)
    {
        transp = 0;
        if (level_sector[s].flags & SECFLAG_NOMAP)
            continue;
        if (!(level_sector[s].flags & SECFLAG_SEEN))
        {
            if (!gotFullMap)
                continue;
            color = RGB(15, 15, 15);
        }
        else
        {
            color = (level_sector[s].floorLevel - currentHeight) >> 4;
            if (color < 0)
            {
                if (color < -31)
                {
                    color = -31;
                    transp = 1;
                }
                color = RGB(31, 31 + color, 31 + color);
            }
            else if (color > 0)
            {
                if (color > 31)
                {
                    color = 31;
                    transp = 1;
                }
                color = RGB(31 - color, 31 - color, 31);
            }
            else
                color = RGB(31, 31, 31);
        }
        for (w = level_sector[s].firstWall; w <= level_sector[s].lastWall; w++)
        {
            if (!mapColor[w])
            {
                continue;
            }
            getVertex(level_wall[w].v[0], &wallP);
            p1.x = f(wallP.x - cx);
            p1.y = f(wallP.z - cy);
            getVertex(level_wall[w].v[1], &wallP);
            p2.x = f(wallP.x - cx);
            p2.y = f(wallP.z - cy);

            mapLine[0].x = f(p1.x * north.x + p1.y * north.y);
            mapLine[0].y = f(p1.x * east.x + p1.y * east.y);
            mapLine[1].x = f(p2.x * north.x + p2.y * north.y);
            mapLine[1].y = f(p2.x * east.x + p2.y * east.y);
            /* clip */
            if (mapLine[0].x < MINX && mapLine[1].x < MINX)
                continue;
            if (mapLine[0].x > MAXX && mapLine[1].x > MAXX)
                continue;
            if (mapLine[0].y < MINY && mapLine[1].y < MINY)
                continue;
            if (mapLine[0].y > MAXY && mapLine[1].y > MAXY)
                continue;
            if (transp)
                EZ_line(UCLPIN_ENABLE | ECDSPD_DISABLE | COLOR_5 | COMPO_TRANS, color, mapLine, NULL);
            else
                EZ_line(UCLPIN_ENABLE | ECDSPD_DISABLE | COLOR_5 | COMPO_REP, color, mapLine, NULL);
        }
    }

    mapLine[0].x = 0;
    mapLine[0].y = -ARROWR;
    mapLine[1].x = 0;
    mapLine[1].y = +ARROWR;
    EZ_line(ECDSPD_DISABLE | COLOR_5 | COMPO_REP, colors[1], mapLine, NULL);
    mapLine[0].x = -ARROWR;
    mapLine[0].y = 0;
    mapLine[1].x = 0;
    mapLine[1].y = -ARROWR;
    EZ_line(ECDSPD_DISABLE | COLOR_5 | COMPO_REP, colors[1], mapLine, NULL);
    mapLine[0].x = ARROWR;
    mapLine[0].y = 0;
    mapLine[1].x = 0;
    mapLine[1].y = -ARROWR;
    EZ_line(ECDSPD_DISABLE | COLOR_5 | COMPO_REP, colors[1], mapLine, NULL);
#if 0
#ifndef NDEBUG
 (sint32 sec;
  Sprite *s;
  for (sec=0;sec<level_nmSectors;sec++)
     {for (s=sectorSpriteList[sec];s;s=s->next)
	 (sint32 x,y,tx,ty;
	  sint32 color;
	  XyInt mark[4];
	  x=f(s->pos.x-cx);
	  y=f(s->pos.z-cy);
	  tx=f(x*north.x+y*north.y);
	  ty=f(x*east.x+y*east.y);
	  mark[0].x=tx-2; mark[0].y=ty-2;
	  mark[1].x=tx+2; mark[1].y=ty-2;
	  mark[2].x=tx+2; mark[2].y=ty+2;
	  mark[3].x=tx-2; mark[3].y=ty+2;
	  if (sec==camera->s)
	     color=RGB(5,5,31);
	  else
	     color=RGB(15,31,15);
	  EZ_polygon(UCLPIN_ENABLE|ECDSPD_DISABLE|COMPO_REP|COLOR_5,
		     color,mark,NULL);
	 }
     }
 }
#endif
#endif
#ifndef JAPAN
    if (cheatsEnabled)
    {
        char buffer[160];
        sprintf(buffer, "x:%d y:%d  sector:%d", (sint32)f(camera->pos.x), (sint32)f(camera->pos.z), camera->s);
        drawString(-90, -100, 1, buffer);
    }
#endif
}
