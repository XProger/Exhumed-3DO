#include <machine.h>

#include <sega_spr.h>
#include <sega_scl.h>
#include <sega_int.h>
#include <sega_mth.h>
#include <sega_sys.h>
#include <sega_dbg.h>
#include <sega_per.h>
#include <sega_dma.h>
#include <string.h>

#include "pic.h"
#include "util.h"
#include "spr.h"
#include "file.h"
#include "slevel.h"
#include "sequence.h"

#include "art.h"
#include "dma.h"

#define COMPRESS16BPP 1
#define MIPMAP 1

/* note: mipmaping is incompatible with locked tiles */

#ifndef JAPAN
#if MIPMAP
#define MAXNMPICS 1600
#else
#define MAXNMPICS 800
#endif
#else
#define MAXNMPICS 1000
#endif

static sint32 frameCount;

#define MAXNMVDP2PICS 50
struct _vdp2PicData
{
    sint16 x, y;
    sint16 w, h;
} vdp2PicData[MAXNMVDP2PICS];
static sint32 nmVDP2Pics;

#define PICFLAG_LOCKED 1
#define PICFLAG_RLE 2
#ifdef JAPAN
#define PICFLAG_RLE2 4
#endif
#if MIPMAP
#define PICFLAG_MIP 8
#endif
#define PICFLAG_ANIM 0xf0
typedef struct
{
    void* data; /* data = NULL if not in use */
#if COMPRESS16BPP
    void* pallete;
#endif
    sint32 lastUse;
    sint16 charNm; /* or -1 if not mapped */
    sint8 class;
    uint8 flags;
} Pic;

typedef struct
{ /* static */
    sint32 colorMode, drawWord, width, height, nmSlots, dataSize;
    Pic** slots;
    /* dynamic */
    sint32 picNmBase;
    sint8 nmSwaps;
} ClassType;

static Pic *__TILE8BPPSPACE[60 + 1], *__TILE16BPPSPACE[48 + 1];
static Pic *__VDPSPACE[2], *__TILESMALL16BPPSPACE[40 + 1], *__TILESMALL8BPPSPACE[20];

#ifdef JAPAN
static Pic* __JFONTSPACE[80];
#endif

ClassType classType[NMCLASSES] = { { COLOR_5, UCLPIN_ENABLE | COLOR_5 | HSS_ENABLE | ECD_DISABLE | DRAW_GOURAU, 64, 64, 0, 64 * 64 * 2, __TILE16BPPSPACE }, { COLOR_4, UCLPIN_ENABLE | COLOR_4 | HSS_ENABLE | ECD_DISABLE, 64, 64, 0, 64 * 64, __TILE8BPPSPACE }, { 0, 0, 0, 0, 1, sizeof(struct _vdp2PicData), __VDPSPACE }, { COLOR_5, UCLPIN_ENABLE | COLOR_5 | HSS_ENABLE | ECD_DISABLE | DRAW_GOURAU, 32, 32, 0, 32 * 32 * 2, __TILESMALL16BPPSPACE }, { COLOR_4, UCLPIN_ENABLE | COLOR_4 | HSS_ENABLE | ECD_DISABLE, 32, 32, 0, 32 * 32, __TILESMALL8BPPSPACE }
#ifdef JAPAN
    ,
    { COLOR_1, UCLPIN_ENABLE | COLOR_5 | HSS_ENABLE | ECD_DISABLE, 24, 18, 0, 24 * 18 / 2, __JFONTSPACE }
#endif
};

Pic pics[MAXNMPICS];
static sint32 nmPics;
static uint16* palletes;

#if MIPMAP
sint8 mippedSlot[MAXNMPICS];
#endif

#define MAXNMANIMSETS 15
/* these are chunk indexes */
static sint32 animTileStart[MAXNMANIMSETS];
static sint32 animTileEnd[MAXNMANIMSETS];
static sint32 nmAnimTileSets;
static sint32 animTileChunk[MAXNMANIMSETS];

void advanceWallAnimations(void)
{
    sint32 i;
    static sint32 flip;
    flip = !flip;
    if (flip)
        return;
    for (i = 0; i < nmAnimTileSets; i++)
    {
        if ((++animTileChunk[i]) >= animTileEnd[i])
            animTileChunk[i] = animTileStart[i];
    }
}

void markAnimTiles(void)
{
    static sint32 animObjectList[] = { OT_ANIM_CHAOS1, OT_ANIM_CHAOS2, OT_ANIM_CHAOS3, OT_ANIM_LAVA1, OT_ANIM_LAVA2, OT_ANIM_LAVA3, OT_ANIM_LAVAFALL, OT_ANIM_LAVAPO1, OT_ANIM_LAVAPO2, OT_ANIM_TELEP1, OT_ANIM_TELEP2, OT_ANIM_TELEP3, OT_ANIM_TELEP4, OT_ANIM_TELEP5, OT_ANIM_LAVAHEAD, OT_ANIM_FORCEFIELD, OT_ANIM_WSAND, OT_ANIM_WBRICK, OT_ANIM_SWAMP, OT_ANM1, OT_ANM2, OT_ANM3, OT_ANM4, OT_ANM5, OT_ANM6, OT_ANM7, OT_ANM8, OT_ANM9, OT_ANM10, OT_ANM11, OT_ANM12, 0 };
    sint32 i, seq, frame, tile;
    nmAnimTileSets = 0;
    for (i = 0; animObjectList[i]; i++)
    {
        if (level_sequenceMap[animObjectList[i]] < 0)
            continue;
        assert(nmAnimTileSets < MAXNMANIMSETS);
        seq = level_sequenceMap[animObjectList[i]];
        animTileStart[nmAnimTileSets] = level_frame[level_sequence[seq]].chunkIndex;
        animTileEnd[nmAnimTileSets] = level_frame[level_sequence[seq + 1]].chunkIndex;

        for (frame = level_sequence[seq]; frame < level_sequence[seq + 1]; frame++)
        {
            tile = level_chunk[level_frame[frame].chunkIndex].tile;
            pics[tile].flags = (pics[tile].flags & 0xf) | ((nmAnimTileSets + 1) << 4);
        }
        nmAnimTileSets++;
    }
    for (i = 0; i < MAXNMANIMSETS; i++)
        animTileChunk[i] = animTileStart[i];
}

void setDrawModeBit(sint32 class, sint32 bit, sint32 onOff)
{
    if (onOff)
        classType[class].drawWord |= bit;
    else
        classType[class].drawWord &= ~bit;
}

void pic_nextFrame(sint32* swaps, sint32* used)
{
#ifndef NDEBUG
    sint32 i, j;
    if (swaps)
        for (i = 0; i < NMCLASSES; i++)
        {
            swaps[i] = classType[i].nmSwaps;
            classType[i].nmSwaps = 0;
        }
    if (used)
        for (i = 0; i < NMCLASSES; i++)
        {
            used[i] = 0;
            for (j = 0; j < classType[i].nmSlots; j++)
                if (classType[i].slots[j] && classType[i].slots[j]->lastUse == frameCount)
                    used[i]++;
        }
#endif
    frameCount++;
}

sint32 getPicClass(sint32 p)
{
    return pics[p].class;
}

uint8* getPicData(sint32 p)
{
    return pics[p].data;
}

static uint8* rleBuffer;

/* returns new pic num */
sint32 initPicSystem(sint32 _picNmBase, sint32* classSizes)
{
    sint32 i;
    enum Class c;
    nmPics = 0;
    frameCount = 0;
    palletes = NULL;
    nmVDP2Pics = 0;
    rleBuffer = (uint8*)0x6001000; /*mem_malloc(1,4096);*/
    for (c = 0; c < NMCLASSES; c++)
    {
        classType[c].nmSlots = *(classSizes++);
        classType[c].picNmBase = _picNmBase;
        classType[c].nmSwaps = 0;
        for (i = 0; i < classType[c].nmSlots; i++)
        {
            classType[c].slots[i] = NULL;
            if (classType[c].width == 0)
                continue;
            EZ_setChar(_picNmBase++, classType[c].colorMode, classType[c].width, classType[c].height, NULL);
            assert(EZ_charNoToVram(_picNmBase - 1));
        }
        classType[c].slots[i] = NULL; /* set sentinel */
    }
    assert(*classSizes == -1);
    for (i = 0; i < MAXNMPICS; i++)
    {
        pics[i].data = NULL;
        pics[i].charNm = -1;
    }
    return _picNmBase;
}

void resetPics(void)
{
    sint32 c, i;
    for (i = 0; i < MAXNMPICS; i++)
        if (!(pics[i].flags & PICFLAG_LOCKED))
            pics[i].charNm = -1;
    for (c = 0; c < NMCLASSES; c++)
    {
        for (i = 0; i < classType[c].nmSlots; i++)
            if (classType[c].slots[i] && !(classType[c].slots[i]->flags & PICFLAG_LOCKED))
                classType[c].slots[i] = NULL;
    }
}

#if MIPMAP
uint16 mipbuff[1024 * 4];
#endif

static void map(Pic* p)
{
    sint32 oldest, oldTime, index;
    Pic **array, **s;
    uint8* srcData;
#if COMPRESS16BPP
    uint16 pbuff[1024 * 4];
#endif
    checkStack();
    validPtr(p);
    assert(p->class != TILEVDP);
    assert(p->charNm == -1);
    assert(p->class >= 0 && p->class <= NMCLASSES);
    /* find the oldest slot in the apropriate array */
    oldest = -1;
    oldTime = frameCount + 1;
    array = classType[(sint32)p->class].slots;

#ifndef NDEBUG
    for (s = array; *s; s++)
        assert(*s != p);
#endif
    for (s = array; *s; s++)
        if (!((*s)->flags & PICFLAG_LOCKED) && (*s)->lastUse < oldTime)
        {
            oldTime = (*s)->lastUse;
            oldest = s - array;
        }
    if (s - array < classType[(sint32)p->class].nmSlots)
    { /* we found an empty slot. */
        index = s - array;
    }
    else
    { /* otherwise we have to knock one out */
        assert(oldest >= 0);
        index = oldest;
        classType[(sint32)p->class].slots[oldest]->charNm = -1;
#ifndef NDEBUG
        classType[(sint32)p->class].nmSwaps++;
#endif
    }

    classType[(sint32)p->class].slots[index] = p;
    p->charNm = index + classType[(sint32)p->class].picNmBase;

    if (p->flags & PICFLAG_RLE)
    {
        register sint32 outSize;
        register sint32 i;
        register uint8* inPos;
        sint32 nmPixels;
        outSize = 0;
        inPos = p->data;
        if (p->class == TILESMALL8BPP)
            nmPixels = 32 * 32;
        else
            nmPixels = 64 * 64;
        while (outSize < nmPixels)
        { /* decode blank space */
            i = *(inPos++);
            for (; i; i--)
                rleBuffer[outSize++] = 0;
            /* decode not blank space */
            i = *(inPos++);
            for (; i; i--)
                rleBuffer[outSize++] = *(inPos++);
        }

        assert(outSize == nmPixels);
        srcData = rleBuffer;
    }
    else
        srcData = p->data;
#ifdef JAPAN
    if (p->flags & PICFLAG_RLE2)
    {
        register sint32 outSize;
        register sint32 i;
        register uint8* inPos;
        sint32 nmPixels;
        outSize = 0;
        inPos = p->data;
        nmPixels = 24 * 18;
        for (i = 0; i < nmPixels >> 1; i++)
            rleBuffer[i] = 0;
        while (outSize < nmPixels)
        { /* decode blank space */
            i = *(inPos++);
            for (; i; i--)
            {
                if (outSize & 1)
                    rleBuffer[(outSize >> 1)] |= 0x01;
                else
                    rleBuffer[(outSize >> 1)] |= 0x10;
                outSize++;
            }
            /* decode not blank space */
            outSize += *(inPos++);
        }
        assert(outSize == nmPixels);
        srcData = rleBuffer;
    }
#endif

#if COMPRESS16BPP
    if (p->pallete)
    {
        register sint32 i;
        for (i = 0; i < classType[(sint32)p->class].dataSize >> 1; i++)
            pbuff[i] = ((uint16*)p->pallete)[(sint32)srcData[i]];
        srcData = (uint8*)pbuff;
    }
#endif

#if MIPMAP
    if (p->flags & PICFLAG_MIP)
    { /* convert the image in srcData to a mip down */
        sint32 x, y;
        sint32 p, src;
        uint16* ssrc = (uint16*)srcData;
        for (y = 0, p = 0, src = 0; y < 32; y++, p += 32, src += 64)
            for (x = 0; x < 32; x++, p++, src += 2)
            {
                uint16 temp1 = ssrc[src];
                uint16 temp2 = ssrc[src + 1];
                uint16 temp3 = ssrc[src + 64];
                uint16 temp4 = ssrc[src + 64 + 1];
                uint16 avg = (((temp1 & 0x001f) + (temp2 & 0x001f) + (temp3 & 0x001f) + (temp4 & 0x001f)) >> 2) | ((((temp1 & 0x03e0) + (temp2 & 0x03e0) + (temp3 & 0x03e0) + (temp4 & 0x03e0)) >> 2) & 0x03e0) | ((((temp1 & 0x7c00) + (temp2 & 0x7c00) + (temp3 & 0x7c00) + (temp4 & 0x7c00)) >> 2) & 0x7c00) | 0x8000;

                mipbuff[p] = avg;
                mipbuff[p + 32] = avg;
                mipbuff[p + 32 * 64] = avg;
                mipbuff[p + 32 * 64 + 32] = avg;
            }
        srcData = (sint8*)mipbuff;
    }
#endif

    {
#ifdef TODO // VRAM copy
        uint8* pos = (uint8*)((EZ_charNoToVram(p->charNm) << 3) + 0x25c00000);

        /* DMA_ScuMemCopy(pos,srcData,classType[(sint32)p->class].dataSize); */

        dmaMemCpy(srcData, pos, classType[(sint32)p->class].dataSize);

        /*  qmemcpy(pos,srcData,classType[(sint32)p->class].dataSize); */
#endif
    }
}

/* if a pic is locked, its memory may be free'd after this call */
sint32 addPic(enum Class class, void* data, void* pallete, sint32 flags)
{
    sint32 p;
    p = nmPics++;
    assert(p < MAXNMPICS);
#if COMPRESS16BPP
    pics[p].pallete = pallete;
#endif
    pics[p].data = data;
    pics[p].lastUse = 0;
    pics[p].class = class;
    pics[p].charNm = -1;
    pics[p].flags = flags;
    if (flags & PICFLAG_LOCKED)
        map(pics + p);
    return p;
}

#if MIPMAP
sint32 createMippedPics(void)
{
    sint32 p, firstMip;
    firstMip = nmPics;
    for (p = 0; p < firstMip; p++)
    {
        pics[nmPics] = pics[p];
        pics[nmPics].flags |= PICFLAG_MIP;
        nmPics++;
        assert(nmPics < MAXNMPICS);
    }
    return firstMip;
}
#endif

sint32 mapPic(sint32 picNm)
{
    Pic* p = pics + picNm;
    assert(picNm >= 0);
    assert(picNm < nmPics);
    if (p->flags & PICFLAG_ANIM)
    {
        picNm = level_chunk[animTileChunk[(p->flags >> 4) - 1]].tile;
        p = pics + picNm;
    }
    assert((p->flags & PICFLAG_LOCKED) || p->data);
    assert(p->class != TILEVDP);
    if (p->charNm == -1)
        map(p);
    p->lastUse = frameCount;
    assert(p->charNm >= 0);
    return p->charNm;
}

static sint32 vxmin, vymin, vxmax, vymax, vx, vy;

void updateVDP2Pic(void)
{
    SCL_Open(SCL_NBG0);
    SCL_MoveTo(vx << 16, vy << 16, 0);
    SCL_Close();
    SCL_SetWindow(SCL_W0, 0, SCL_NBG0, 0xfffffff, vxmin, vymin, vxmax, vymax);
}

void displayVDP2Pic(sint32 picNm, sint32 xo, sint32 yo)
{
    sint32 xmin, ymin, xmax, ymax;
    struct _vdp2PicData* pd = (struct _vdp2PicData*)pics[picNm].data;
    vx = pd->x - xo;
    vy = pd->y - yo;
#if 0
 SCL_Open(SCL_NBG0);
 SCL_MoveTo((pd->x-xo)<<16,(pd->y-yo)<<16,0);
 SCL_Close();
#endif
    xmin = xo;
    ymin = yo;
    if (xmin < 0)
        xmin = 0;
    if (ymin < 0)
        ymin = 0;
    xmax = xo + pd->w - 1;
    ymax = yo + pd->h - 1;
    if (xmax > 320)
        xmax = 320;
    if (ymax > 210)
        ymax = 210;

    /* xmin=0; ymin=0; xmax=320; ymax=240; */
    vxmin = xmin;
    vymin = ymin;
    vxmax = xmax;
    vymax = ymax;
#if 0
 SCL_SetWindow(SCL_W0,0,SCL_NBG0,0xfffffff,xmin,ymin,
	       xmax,ymax);
#endif
}

void delay_dontDisplayVDP2Pic(void)
{
    vxmin = 0;
    vymin = 0;
    vxmax = 0;
    vymax = 0;
}

void dontDisplayVDP2Pic(void)
{
    SCL_SetWindow(SCL_W0, 0, SCL_NBG0, 0xfffffff, 0, 0, 0, 0);
    vxmin = 0;
    vymin = 0;
    vxmax = 0;
    vymax = 0;
}

static void load16BPPTile(sint32 fd, sint32 lock)
{
    sint16 width, height, palNm;
#if !COMPRESS16BPP
    sint32 i;
#endif
    uint8* buffer;
    uint8* b;
    sint16* pal;
    width = 64;
    height = 64;
#if !COMPRESS16BPP
    buffer = (uint8*)mem_malloc(1, 2 * width * height);
#endif
    b = (uint8*)mem_malloc(1, width * height);
    assert(b);
    fs_read(fd, (sint8*)&palNm, 2);

    palNm = FS_SHORT(&palNm);

    assert(palletes);
    pal = palletes + 256 * palNm + 1;
    fs_read(fd, b, width * height);
#if COMPRESS16BPP
    buffer = b;
#else
    assert(buffer);
    for (i = 0; i < width * height; i++)
    {
        buffer[i * 2] = pal[b[i]] >> 8;
        buffer[i * 2 + 1] = pal[b[i]] & 0xff;
    }
    mem_free(b);
#endif
    addPic(TILE16BPP, buffer, pal, lock ? PICFLAG_LOCKED : 0);
    if (lock)
        mem_free(buffer);
}

static void loadSmall16BPPTile(sint32 fd, sint32 lock)
{
    sint16 width, height, palNm;
#if !COMPRESS16BPP
    sint32 i;
#endif
    uint8* buffer;
    uint8* b;
    sint16* pal;
    width = 32;
    height = 32;
#if !COMPRESS16BPP
    buffer = (uint8*)mem_malloc(1, 2 * width * height);
#endif
    b = (uint8*)mem_malloc(1, width * height);
    assert(b);
    fs_read(fd, (sint8*)&palNm, 2);

    palNm = FS_SHORT(&palNm);

    assert(palletes);
    pal = palletes + 256 * palNm + 1;
    fs_read(fd, b, width * height);
#if COMPRESS16BPP
    buffer = b;
#else
    assert(buffer);
    for (i = 0; i < width * height; i++)
    {
        buffer[i * 2] = pal[b[i]] >> 8;
        buffer[i * 2 + 1] = pal[b[i]] & 0xff;
    }
    mem_free(b);
#endif
    addPic(TILESMALL16BPP, buffer, pal, lock ? PICFLAG_LOCKED : 0);
    if (lock)
        mem_free(buffer);
}

static void load8BPPRLETile(sint32 fd, sint32 lock)
{
    uint8* buffer;
    sint16 size, palNm;
    fs_read(fd, (sint8*)&palNm, 2);
    fs_read(fd, (sint8*)&size, 2);

    palNm = FS_SHORT(&palNm);
    size = FS_SHORT(&size);

    assert(size);
    buffer = (sint8*)mem_malloc(0, size);
    assert(buffer);
    fs_read(fd, buffer, size);
    addPic(TILE8BPP, buffer, NULL, (lock ? PICFLAG_LOCKED : 0) | PICFLAG_RLE);
    if (lock)
        mem_free(buffer);
}

static void loadSmall8BPPRLETile(sint32 fd, sint32 lock)
{
    uint8* buffer;
    sint16 size, palNm;
    fs_read(fd, (sint8*)&palNm, 2);
    fs_read(fd, (sint8*)&size, 2);

    palNm = FS_SHORT(&palNm);
    size = FS_SHORT(&size);

    assert(size);
    buffer = (sint8*)mem_malloc(0, size);
    assert(buffer);
    fs_read(fd, buffer, size);
    addPic(TILESMALL8BPP, buffer, NULL, (lock ? PICFLAG_LOCKED : 0) | PICFLAG_RLE);
    if (lock)
        mem_free(buffer);
}

static void load16BPPRLETile(fd, lock)
{
    uint8* buffer;
    sint16 size, palNm;
    sint16* pal;
    fs_read(fd, (sint8*)&palNm, 2);
    fs_read(fd, (sint8*)&size, 2);

    palNm = FS_SHORT(&palNm);
    size = FS_SHORT(&size);

    assert(size);
    buffer = (sint8*)mem_malloc(0, size);
    fs_read(fd, buffer, size);
    pal = palletes + 256 * palNm + 1;
    addPic(TILE16BPP, buffer, pal, (lock ? PICFLAG_LOCKED : 0) | PICFLAG_RLE);
    if (lock)
        mem_free(buffer);
}

void loadPalletes(sint32 fd)
{
    sint32 size, i, j;
    uint16* colorRam = (uint16*)SCL_COLRAM_ADDR;
    fs_read(fd, (sint8*)&size, 4);

    size = FS_INT(&size);

    assert(size > 0 && size < 1024 * 1024);
    palletes = (uint16*)mem_malloc(
#if COMPRESS16BPP
        1
#else
        0
#endif
        ,
        size);
    assert(palletes);
    fs_read(fd, (sint8*)palletes, size);
    /* ... load the object pallete into c-ram */
    {
        uint16* objectPal;
        uint16 tempSpace[256];
        sint32 c;
        objectPal = palletes + 256 * (*palletes) + 1;
        objectPal[255] = 0xffff;

        for (i = 0; i < 256; i++)
            colorRam[i] = objectPal[i];
        /* SCL_SetColRam(0,0,256,objectPal); */

        for (i = 1; i < NMOBJECTPALLETES; i++)
        {
            sint32 r, g, b;
            for (c = 0; c < 256; c++)
            {
                r = objectPal[c] & 0x1f;
                g = (objectPal[c] >> 5) & 0x1f;
                b = (objectPal[c] >> 10) & 0x1f;
                r -= i;
                if (r < 0)
                    r = 0;
                g -= i;
                if (g < 0)
                    g = 0;
                b -= i;
                if (b < 0)
                    b = 0;
                tempSpace[c] = RGB(r, g, b);
            }
            for (j = 0; j < 256; j++)
                colorRam[i * 256 + j] = tempSpace[j];
            /* SCL_SetColRam(0,i*256,256,tempSpace); */
        }
        /* make flash pallete */
        for (c = 0; c < 256; c++)
            tempSpace[c] = 0xffff;
        tempSpace[0] = 0x8000;

        for (j = 0; j < 256; j++)
            colorRam[NMOBJECTPALLETES * 256 + j] = tempSpace[j];
        /* SCL_SetColRam(0,NMOBJECTPALLETES*256,256,tempSpace); */
    }
}

/* return # of tiles loaded */
sint32 loadTileSet(sint32 fd, sint32 lock)
{
    sint32 nmTiles, i;
    sint16 flags;

    fs_read(fd, (sint8*)&nmTiles, 4);

    nmTiles = FS_INT(&nmTiles);

    for (i = 0; i < nmTiles; i++)
    {
        fs_read(fd, (sint8*)&flags, 2);

        flags = FS_SHORT(&flags);

        switch (flags)
        {
            case (TILEFLAG_64x64 | TILEFLAG_16BPP | TILEFLAG_PALLETE):
                load16BPPTile(fd, lock);
                break;
            case (TILEFLAG_VDP2):
            {
                struct _vdp2PicData *pic = vdp2PicData + nmVDP2Pics;

                assert(nmVDP2Pics < MAXNMVDP2PICS);
                fs_read(fd, (sint8*)pic, 8);

                pic->x = FS_SHORT(&pic->x);
                pic->y = FS_SHORT(&pic->y);
                pic->w = FS_SHORT(&pic->w);
                pic->h = FS_SHORT(&pic->h);

                addPic(TILEVDP, vdp2PicData + nmVDP2Pics, NULL, 0);
                nmVDP2Pics++;
                break;
            }
            case (TILEFLAG_64x64 | TILEFLAG_8BPP | TILEFLAG_RLE | TILEFLAG_PALLETE):
                load8BPPRLETile(fd, lock);
                break;
            case (TILEFLAG_32x32 | TILEFLAG_8BPP | TILEFLAG_RLE | TILEFLAG_PALLETE):
                loadSmall8BPPRLETile(fd, lock);
                break;
            case (TILEFLAG_32x32 | TILEFLAG_16BPP | TILEFLAG_PALLETE):
                loadSmall16BPPTile(fd, lock);
                break;
            case (TILEFLAG_64x64 | TILEFLAG_16BPP | TILEFLAG_PALLETE | TILEFLAG_RLE):
                load16BPPRLETile(fd, lock);
                break;
            default:
                assert(0);
                break;
        }
    }
    return nmTiles;
}

/* returns # of weapon tiles loaded */
sint32 loadWeaponTiles(sint32 fd)
{
    return loadTileSet(fd, 0);
}

void loadTiles(sint32 fd)
{
    loadPalletes(fd);
    loadTileSet(fd, 0);
}

sint32 loadPicSetAsPics(sint32 fd, sint32 class)
{
    uint32* datas[50];
    uint16* palletes[50];
    sint32 nmSetPics, i, x, y, picBase;
    picBase = nmPics;
    nmSetPics = loadPicSet(fd, palletes, datas, 50);

    for (i = 0; i < nmSetPics; i++)
    {
        switch (class)
        {
            case TILESMALL16BPP:
            {
                uint16 buffer[32 * 32];
                sint32 width = datas[i][0];
                sint32 height = datas[i][1];
                uint8 *src = (uint8*)(datas[i] + 2);

                memset(buffer, 0, 32 * 32 * 2);
                assert(!(((sint32)datas[i]) & 0x3));
                assert(!(((sint32)palletes[i]) & 0x1));
                for (y = 0; y < height; y++)
                    for (x = 0; x < width; x++)
                        buffer[y * 32 + x] = palletes[i][*src++];
                addPic(TILESMALL16BPP, buffer, NULL, PICFLAG_LOCKED);
                break;
            }
            case TILE16BPP:
                /* this does something totaly different from the SMALL16BPP case.
                   sorry. */
                addPic(TILE16BPP, datas[i] + 2, palletes[i], 0);
                break;
        }

    #ifdef DUMP_PICS
        save_tga(i, (uint8*)datas[i], palletes[i], class);
    #endif
    }
    return picBase;
}

#ifdef DUMP_PICS
#include <stdio.h>

typedef struct {
    uint8 id_length;
    uint8 color_map_type;
    uint8 image_type;
    uint8 data2[9];
    uint16 width;
    uint16 height;
    uint8 bpp;
    uint8 desc;
} TGA_HEADER;

void save_tga(sint32 id, uint8 *data, uint16 *pal16, sint32 pic_class)
{
    TGA_HEADER header;
    uint32 zero = 0;
    FILE *f;
    sint32 x, y, size;
    char name[16];
    uint32 pal[256];
    sint32 width;
    sint32 height;

    sprintf(name, "dump\\%d.TGA", id);

    for (x = 0; x < 256; x++)
    {
        uint16 c = FS_SHORT(pal16 + x);
        uint8 r = c & 31;
        uint8 g = (c >> 5) & 31;
        uint8 b = (c >> 10) & 31;

        r <<= 3;
        g <<= 3;
        b <<= 3;

        pal[x] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    width = ((sint32*)(data))[0];
    height = ((sint32*)(data))[1];
    data += 8;

    memset(&header, 0, sizeof(header));
    header.image_type = 2;
    header.width = width;
    header.height = height;
    header.bpp = 32;
    header.desc = 1 << 5; // flip vertical

    f = fopen(name, "wb");
    assert(f);
    fwrite(&header, sizeof(header), 1, f);

    size = width * height;
    if (pic_class == NMCLASSES)
    {
        while (size)
        {
            y = *data++;
            while (y--)
            {
                fwrite(&zero, 1, 4, f);
                size--;
            }
            y = *data++;
            while (y--)
            {
                fwrite(pal + *data++, 1, 4, f);
                size--;
            }
        }
    }
    else
    {
        while (size--)
        {
            fwrite(pal + *data++, 1, 4, f);
        }
    }

    fclose(f);
}
#endif