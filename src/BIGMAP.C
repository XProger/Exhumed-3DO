#include <machine.h>

#include <libsn.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sega_spr.h"
#include "sega_scl.h"
#include "sega_int.h"
#include "sega_mth.h"
#include "sega_sys.h"
#include "sega_dbg.h"
#include "sega_per.h"
#include "sega_cdc.h"
#include "sega_snd.h"
#include "sega_gfs.h"

#include "pic.h"
#include "file.h"
#include "util.h"
#include "spr.h"
#include "print.h"
#include "v_blank.h"
#include "art.h"
#include "local.h"
#include "gamestat.h"
#include "sound.h"

static sint32 levelPos[NMLEVELS][2] = {

    { 419, 292 },
    { 572, 300 },
    { 368, 202 },
    { 333, 234 },
    { 381, 156 },
    { 376, 405 },
    { 487, 416 },
    { 564, 418 },
    { 256, 457 },
    { 77, 417 },
    { 61, 320 },
    { 69, 195 },
    { 320, 70 },
    { 63, 111 },
    { 417, 431 },
    { 245, 72 },
    { 234, 179 },
    { 239, 313 },
    { 573, 178 },
    { 575, 71 },
    { 435, 70 },
    { 169, 469 },
    { 279, 221 },
    { 236, 285 },
    { 158, 469 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },

};

typedef struct
{
    sint8 link[4];
    char* levelFile;
} LevData;

/* up, right, down, left */
static LevData levelGraph[NMLEVELS] = {
    /* 0*/ { { 2, 1, 5, 3 }, "+KARNAK.LEV" },
    /* 1*/ { { 18, -1, -1, -1 }, "+SANCTUAR.LEV" },
    /* 2*/ { { 4, -1, -1, -1 }, "+PASS.LEV" },
    /* 3*/ { { -1, -1, -1, -1 }, "+TOMB.LEV" },
    /* 4*/ { { -1, -1, -1, 16 }, "+SHRINE.LEV" },

    /* 5*/ { { -1, 6, -1, 8 }, "+MINES.LEV" },
    /* 6*/ { { -1, 7, -1, 5 }, "+SETPALAC.LEV" },
    /* 7*/ { { -1, -1, -1, -1 }, "+SETARENA.LEV" },
    /* 8*/ { { -1, -1, -1, 21 }, "+CAVERN.LEV" },
    /* 9*/ { { 10, 21, -1, -1 }, "+THOTH.LEV" },

    /*10*/ { { 11, -1, -1, -1 }, "+CHAOS.LEV" },
    /*11*/ { { 13, -1, -1, -1 }, "+COLONY.LEV" },
    /*12*/ { { -1, 20, -1, 15 }, "+SELPATH.LEV" },
    /*13*/ { { -1, -1, -1, -1 }, "+KILENTRY.LEV" },
    /*14*/ { { -1, -1, -1, -1 }, "+QUARRY.LEV" },

    /*15*/ { { -1, -1, -1, -1 }, "+SELBUROW.LEV" },
    /*16*/ { { -1, -1, 17, -1 }, "+MAGMA.LEV" },
    /*17*/ { { -1, -1, -1, -1 }, "+PEAK.LEV" },
    /*18*/ { { 19, -1, -1, -1 }, "+MARSH.LEV" },
    /*19*/ { { -1, -1, -1, 20 }, "+SUNKEN.LEV" },

    /*20*/ { { -1, -1, -1, -1 }, "+SLAVCAMP.LEV" },
    /*21*/ { { -1, -1, -1, -1 }, "+GORGE.LEV" },
    /*22*/ { { -1, -1, -1, -1 }, "+TEST.LEV" },
    /*23*/ { { -1, -1, -1, -1 }, "+KILMAAT1.LEV" },
    /*24*/ { { -1, -1, -1, -1 }, "+KILMAAT2.LEV" },

    /*25*/ { { -1, -1, -1, -1 }, "+KILMAAT3.LEV" },
    /*26*/ { { -1, -1, -1, -1 }, "+KILMAAT4.LEV" },
    /*27*/ { { -1, -1, -1, -1 }, "+KILMAAT5.LEV" },
    /*28*/ { { -1, -1, -1, -1 }, "+KILMAAT6.LEV" },
    /*29*/ { { -1, -1, -1, -1 }, "+KILARENA.LEV" },

    /*30*/ { { -1, -1, -1, -1 }, "+TOMBEND.LEV" },
};

char* getLevelName(sint32 lNm)
{
    return levelGraph[lNm].levelFile;
}

static void completeGraph(void)
{
    sint32 i;
    /* complete graph */
    for (i = 0; i < NMLEVELS; i++)
    {
        if (levelGraph[i].link[0] > i)
            levelGraph[(sint32)levelGraph[i].link[0]].link[2] = i;
        if (levelGraph[i].link[1] > i)
            levelGraph[(sint32)levelGraph[i].link[1]].link[3] = i;
        if (levelGraph[i].link[2] > i)
            levelGraph[(sint32)levelGraph[i].link[2]].link[0] = i;
        if (levelGraph[i].link[3] > i)
            levelGraph[(sint32)levelGraph[i].link[3]].link[1] = i;
    }
}

sint32 getMapLink(sint32 fromLevel, sint32 direction)
{
    completeGraph();
    return levelGraph[fromLevel].link[direction];
}

static sint32 firstMapTile;

sint32 loadMapTiles(sint32 fd)
{ /* returns # of map tiles loaded */
    sint32 nmTiles, i, j;
    uint8* b;
    sint16 flags;
    firstMapTile = -1;
    fs_read(fd, (sint8*)&nmTiles, 4);

    nmTiles = FS_INT(&nmTiles);

    assert(nmTiles < 100 && nmTiles >= 0);
    for (i = 0; i < nmTiles; i++)
    {
        fs_read(fd, (sint8*)&flags, 2);

        flags = FS_SHORT(&flags);

        b = (uint8*)mem_malloc(1, 64 * 64 * 2);
        fs_read(fd, b, 64 * 64 * 2);
        j = addPic(TILE16BPP, b, NULL, 0);
        if (firstMapTile == -1)
            firstMapTile = j;
    }
    return nmTiles;
}

#define NMCODEZ 3
/* start all codez with tl */
static char* codez[NMCODEZ] = {
    "DRLUDRLUDRLDUL", /* enable full health/ammo */
    "ZXYYXYXXYZZYXZA", /* kilmaat */
    "YYZZXXZYXZYXYXZ", /* force map move */
};
#define MAPTILEWIDTH 10
#define MAPTILEHEIGHT 8
/* returns next level to load */
sint32 runMap(sint32 currentLevel)
{
    sint32 frame = 0;
    sint32 x, y, i, j;
    sint32 input, lastInput, changeInput;
    sint32 startVel[2];
    sint32 selX, selY;
    sint32 desiredXOffs, desiredYOffs;
    sint32 xoffs, yoffs;
    sint32 newPos[2], oldPos[2], time, haveNewPos;
    sint32 selectedLevel;
    sint32 fadeCount, mapFadeDir;
    sint32 arrowOffs, eyeFrame, eyeClock;
    sint32 codePos[NMCODEZ];
    fix32 scaleFactor;
    sint8 cheatMove = 0;
    XyInt pos;
    XyInt poly[4];

    for (i = 0; i < NMCODEZ; i++)
        codePos[i] = 0;
    completeGraph();
    /* setup stuff */
    fadePos = 0;
    fadeEnd = -256;
    fadeDir = -4;

    mem_init();
    stopCD();
    initSound();
    {
        sint32 fd = fs_open("+MAP.DAT"); /* start read ahead of file */
#ifdef TODO
        while (fadeDir)
            ;
#endif
        dontDisplayVDP2Pic();
        displayEnable(0);

        EZ_initSprSystem(1248, 4, 1024, 0, 0x8000);
        SPR_SetTvMode(SPR_TV_NORMAL, SPR_TV_320X240, OFF);
        displayEnable(0);

#ifndef JAPAN
        i = initFonts(0, 7);
        initPicSystem(i, ((sint32[]) { 40, 0, 0, 36, 0, -1 }));
#else
        initPicSystem(0, ((sint32[]) { 40, 0, 0, 36, 0, 30, -1 }));
#endif
        loadPicSetAsPics(fd, TILESMALL16BPP);
        loadSound(fd);
        loadSound(fd);
        loadMapTiles(fd);
        fs_close(fd);
    }
#ifdef JAPAN
    loadJapanFontPics();
#endif

    if (enable_music)
        playCDTrack(mapMusic, 1);
    SCL_SetFrameInterval(0xfffe);
    SPR_SetTvMode(SPR_TV_NORMAL, SPR_TV_320X240, OFF);
    displayEnable(0);

    selectedLevel = currentLevel;
    xoffs = 0;
    yoffs = 0;
    newPos[0] = oldPos[0] = levelPos[selectedLevel][0] - 160;
    newPos[1] = oldPos[1] = levelPos[selectedLevel][1] - 120;
    time = F(2);

    selX = 0;
    selY = 0;
    startVel[0] = 0;
    startVel[1] = 0;
    input = lastInputSample;
    fadeCount = 1;
    mapFadeDir = 1;
    arrowOffs = 0;
    eyeFrame = 0;
    eyeClock = 5;
    scaleFactor = F(1) + ((F(1) >> 4) * 31);

    while (1)
    {
        if (!(frame & 0x3f))
        {
            static sint8 tmitLevels[] = { 2, 5, 6, 21, 18, 20, 16, 10, -1 };
            for (i = 0; tmitLevels[i] != -1; i++)
            {
                if (selectedLevel == tmitLevels[i] && !(currentState.inventory & (0x10000 << i)))
                    playSound(0, 0);
            }
        }
        sound_nextFrame();
        EZ_openCommand();
        EZ_localCoord(0, 0);
        EZ_sysClip();
        if (time <= F(1))
        {
            xoffs = f(evalHermite(time, F(oldPos[0]), F(newPos[0]), startVel[0], 0));
            yoffs = f(evalHermite(time, F(oldPos[1]), F(newPos[1]), startVel[1], 0));
        }
        else
        {
            xoffs = levelPos[selectedLevel][0] - 160;
            yoffs = levelPos[selectedLevel][1] - 120;
        }
        /* desiredXOffs=xoffs;
           desiredYOffs=yoffs; */
        desiredXOffs = levelPos[selectedLevel][0] - 160;
        desiredYOffs = levelPos[selectedLevel][1] - 120;
        if (xoffs < 0)
            xoffs = 0;
        if (xoffs > 640 - 320)
            xoffs = 640 - 320;
        if (yoffs < 0)
            yoffs = 0;
        if (yoffs > 512 - 240)
            yoffs = 512 - 240;

        time += F(1) / 32;

        /* draw map tiles */
        if (scaleFactor == F(1))
            for (y = yoffs / 64; y <= (yoffs + 239) / 64; y++)
                for (x = xoffs / 64; x <= (xoffs + 319) / 64; x++)
                {
                    pos.x = x * 64 - xoffs;
                    pos.y = y * 64 - yoffs;
                    EZ_normSpr(0, COLOR_5 | HSS_ENABLE | ECD_DISABLE, 0, mapPic(firstMapTile + y * MAPTILEWIDTH + x), &pos, NULL);
                }
        else
        {
            sint32 scx, scy;
            scx = 160 + (desiredXOffs - xoffs);
            scy = 120 + (desiredYOffs - yoffs);
            for (y = 0; y < MAPTILEHEIGHT; y++)
                for (x = 0; x < MAPTILEWIDTH; x++)
                {
                    poly[0].x = f((x * 64 - xoffs - scx) * scaleFactor) + scx;
                    poly[0].y = f((y * 64 - yoffs - scy) * scaleFactor) + scy;
                    poly[1].x = f(((x + 1) * 64 - xoffs - scx) * scaleFactor) + scx;
                    poly[1].y = f(((y + 1) * 64 - yoffs - scy) * scaleFactor) + scy;
                    if (poly[0].x > 320 || poly[1].x < 0 || poly[0].y > 240 || poly[1].y < 0)
                        continue;
                    EZ_scaleSpr(0, COLOR_5 | HSS_ENABLE | ECD_DISABLE, 0, mapPic(firstMapTile + y * MAPTILEWIDTH + x), poly, NULL);
                }
        }

        if (!fadeCount)
        /* draw level markers */
        {
            for (i = 0; i < NMLEVELS; i++)
            {
                poly[0].x = levelPos[i][0] - xoffs - 12;
                poly[0].y = levelPos[i][1] - yoffs - 5;
                if (i == selectedLevel)
                {
                    sint32 pic;
                    if (eyeClock & 1)
                    {
                        eyeFrame++;
                        if (eyeFrame >= 30)
                            eyeFrame = 0;
                    }
                    if (!eyeClock--)
                    {
                        eyeClock = 4;
                        arrowOffs++;
                        if (arrowOffs >= 4)
                            arrowOffs = -3;
                    }
                    pic = eyeFrame + 4;
                    EZ_normSpr(DIR_NOREV, COMPO_SHADOW | COLOR_5, 0, mapPic(pic), poly, NULL);
                    poly[0].x -= 2;
                    poly[0].y -= 2;
                    EZ_normSpr(DIR_NOREV, COLOR_5, 0, mapPic(pic), poly, NULL);
                    for (j = 0; j < 4; j++)
                    {
                        if (levelGraph[selectedLevel].link[j] != -1)
                        {
                            static struct
                            {
                                sint16 pic, xo, yo, dx, dy;
                            } arrows[] = { { 3, 15, -8, 0, -1 }, { 0, 30, 4, 1, 0 }, { 2, 15, 16, 0, 1 }, { 1, -1, 4, -1, 0 } };
                            if (currentState.levFlags[(sint32)levelGraph[selectedLevel].link[j]] & LEVFLAG_CANENTER)
                            {
                                poly[1].x = poly[0].x + arrows[j].xo + abs(arrowOffs) * arrows[j].dx;
                                poly[1].y = poly[0].y + arrows[j].yo + abs(arrowOffs) * arrows[j].dy;
                                EZ_normSpr(DIR_NOREV, COMPO_SHADOW | COLOR_5, 0, mapPic(arrows[j].pic), poly + 1, NULL);
                                poly[1].x--;
                                poly[1].y--;
                                EZ_normSpr(DIR_NOREV, COLOR_5, 0, mapPic(arrows[j].pic), poly + 1, NULL);
                            }
                            else
                            {
                                poly[1].x = poly[0].x + arrows[j].xo;
                                poly[1].y = poly[0].y + arrows[j].yo;
                                EZ_normSpr(DIR_NOREV, COMPO_TRANS | COLOR_5, 0, mapPic(arrows[j].pic), poly + 1, NULL);
                            }
                        }
                    }
                }
                else
                { /* poly[0].x--;
                     poly[0].y--;
                     SPR_2NormSpr(0,DIR_NOREV,DRAW_GOURAU|COLOR_5,0,mapPic(4),
                     poly,0); */
                }
            }
        }

        /* draw place label */
#ifndef JAPAN
        poly[0].x = 160 - 97;
        poly[0].y = 30;
        poly[1].x = 160 + 97;
        poly[1].y = 30;
        poly[2].x = 160 + 97;
        poly[2].y = 50;
        poly[3].x = 160 - 97;
        poly[3].y = 50;
#else
        poly[0].x = 160 - 105;
        poly[0].y = 30;
        poly[1].x = 160 + 105;
        poly[1].y = 30;
        poly[2].x = 160 + 105;
        poly[2].y = 56;
        poly[3].x = 160 - 105;
        poly[3].y = 56;
#endif
        EZ_polygon(SPD_DISABLE | ECD_DISABLE | COMPO_TRANS, RGB(0, 0, 0), poly, NULL);
        EZ_polygon(SPD_DISABLE | ECD_DISABLE | COMPO_TRANS, RGB(0, 0, 0), poly, NULL);
#ifndef JAPAN
        i = getStringWidth(1, getText(LB_LEVELNAMES, selectedLevel));
        drawString(160 - i / 2, 34, 1, getText(LB_LEVELNAMES, selectedLevel));
#else
        i = getStringWidth(3, getText(LB_LEVELNAMES, selectedLevel));
        drawString(160 - i / 2, 34, 3, getText(LB_LEVELNAMES, selectedLevel));
#endif

        EZ_closeCommand();
        SCL_DisplayFrame();
        pic_nextFrame(NULL, NULL);

        lastInput = input;
        input = lastInputSample;
        changeInput = lastInput ^ input;
        haveNewPos = 0;

#ifdef TODO // input game reset?
        if (!(input & (PER_DGT_A | PER_DGT_B | PER_DGT_C | PER_DGT_S)))
            return -1;
#endif

        if (fadeCount)
        {
            fadeCount++;
            if (mapFadeDir == -1)
            {
                if (fadeCount == 32)
                {
                    EZ_clearScreen();
                    return selectedLevel;
                }
                setMasterVolume(15 - (fadeCount >> 1));
                scaleFactor += F(1) >> 4;
                SCL_SetColOffset(SCL_OFFSET_A, SCL_SP0, -fadeCount << 3, -fadeCount << 3, -fadeCount << 3);
            }
            else
            {
                if (fadeCount == 32)
                {
                    fadeCount = 0;
                    scaleFactor = F(1);
                    SCL_SetColOffset(SCL_OFFSET_A, SCL_SP0, 0, 0, 0);
                }
                else
                {
                    scaleFactor -= F(1) >> 4;
                    SCL_SetColOffset(SCL_OFFSET_A, SCL_SP0, -(31 - fadeCount) << 3, -(31 - fadeCount) << 3, -(31 - fadeCount) << 3);
                }
            }
        }
        else
        {
            { /* do cheats */
                sint32 i;
#define NMCP 9
                static uint16 cp[NMCP] = { PER_DGT_U, PER_DGT_D, PER_DGT_L, PER_DGT_R, PER_DGT_X, PER_DGT_Y, PER_DGT_Z, PER_DGT_A, PER_DGT_S };
                static char cpl[NMCP] = { 'U', 'D', 'L', 'R', 'X', 'Y', 'Z', 'A', 'T' };
                char c = 0;
                for (i = 0; i < NMCP; i++)
                    if ((changeInput & cp[i]) && !(input & cp[i]))
                        c = cpl[i];
                if (c)
                {
                    for (i = 0; i < NMCODEZ; i++)
                    {
                        if (codez[i][codePos[i]] == c)
                        {
                            codePos[i]++;
                            if (codez[i][codePos[i]] == 0)
                            { /* code successful */
                                playSound(0, 1);
                                switch (i)
                                {
                                    case 0:
                                    {
                                        extern sint8 endUserCheatsEnabled;
                                        endUserCheatsEnabled = 1;
                                    }
                                    break;
                                    case 1:
                                        if (selectedLevel == 13)
                                            currentState.gameFlags |= GAMEFLAG_KILENTRYCHEATENABLED;
                                        break;
                                    case 2:
                                        cheatMove = 1;
                                        break;
                                }
                            }
                        }
                        else
                            codePos[i] = 0;
                    }
                }
            }

#if 0
#ifndef NDEBUG
	 if (!(input & PER_DGT_Z))
	    {if (!(input & PER_DGT_D))
		levelPos[selectedLevel][1]++;
	     if (!(input & PER_DGT_U))
		levelPos[selectedLevel][1]--;
	     if (!(input & PER_DGT_L))
		levelPos[selectedLevel][0]--;
	     if (!(input & PER_DGT_R))
		levelPos[selectedLevel][0]++;
	     if ((changeInput & PER_DGT_Y) && !(input & PER_DGT_Y))
		(sint32 fd;
		 char buff[160];
		 fd=PCopen("c:\\sr3\\levelpos.txt",1,0);
		 for (i=0;i<NMLEVELS;i++)
		    {sprintf(buff,"{%d,%d},",levelPos[i][0],levelPos[i][1]);
		     PCwrite(fd,buff,strlen(buff));
		    }
		 PCclose(fd);
		}
	    }
	 else
#endif
#endif
            {
                sint32 dir = -1;
                if ((changeInput & PER_DGT_D) && !(input & PER_DGT_D))
                    dir = 2;
                if ((changeInput & PER_DGT_U) && !(input & PER_DGT_U))
                    dir = 0;
                if ((changeInput & PER_DGT_L) && !(input & PER_DGT_L))
                    dir = 3;
                if ((changeInput & PER_DGT_R) && !(input & PER_DGT_R))
                    dir = 1;

                if (dir != -1 && levelGraph[selectedLevel].link[dir] != -1)
                {
                    if (cheatMove || currentState.levFlags[(sint32)levelGraph[selectedLevel].link[dir]] & LEVFLAG_CANENTER)
                    {
                        currentState.levFlags[(sint32)levelGraph[selectedLevel].link[dir]] |= LEVFLAG_CANENTER;
                        cheatMove = 0;
                        selectedLevel = levelGraph[selectedLevel].link[dir];
                        haveNewPos = 1;
                    }
                }
            }

            if (cheatsEnabled)
                if ((changeInput & (~input)) & (PER_DGT_X))
                {
                    currentState.inventory = INV_SANDALS | INV_MASK | INV_SHAWL | INV_ANKLET | INV_SCEPTER | INV_FEATHER | INV_SWORD | INV_PISTOL | INV_M60 | INV_GRENADE | INV_FLAMER | INV_COBRA | INV_MANACLE | INV_RING;
                    for (i = 0; i < NMLEVELS; i++)
                        currentState.levFlags[i] = LEVFLAG_CANENTER;
                }

            if ((changeInput & (~input)) & (PER_DGT_A | PER_DGT_B | PER_DGT_C))
            {
                if (levelGraph[selectedLevel].levelFile)
                {
                    fadeCount = 1;
                    mapFadeDir = -1;
                    scaleFactor = F(1);
                }
            }
        }

        if (haveNewPos)
        {
            if (time < F(1))
            {
                startVel[0] = evalHermiteD(time, F(oldPos[0]), F(newPos[0]), startVel[0], 0);
                startVel[1] = evalHermiteD(time, F(oldPos[1]), F(newPos[1]), startVel[1], 0);
            }
            else
            {
                startVel[0] = 0;
                startVel[1] = 0;
            }
            oldPos[0] = xoffs;
            oldPos[1] = yoffs;
            time = 0;
            newPos[0] = levelPos[selectedLevel][0] - 160;
            newPos[1] = levelPos[selectedLevel][1] - 120;
            if (newPos[0] < 0)
                newPos[0] = 0;
            if (newPos[1] < 0)
                newPos[1] = 0;
            if (newPos[0] > 640 - 320)
                newPos[0] = 640 - 320;
            if (newPos[1] > 512 - 240)
                newPos[1] = 512 - 240;
        }
        if (frame == 2)
            displayEnable(1);
        frame++;
    }
}
