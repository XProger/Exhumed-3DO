#include <machine.h>

#include <sega_mth.h>
#include <sega_scl.h>
#include <sega_spr.h>
#include <string.h>

#include "util.h"
#include "spr.h"
#include "dma.h"

#define MAXNMCHARS 512
#define CMDBUFFERSIZE 256
#define GOURBUFFERSIZE 256

typedef struct
{
    uint16 addr; /* (address-VRAM START)/8 */
    sint16 xysize; /* (xs/8)<<8 + ysize */
} CharData;

static CharData chars[MAXNMCHARS];
static sint32 nmChars;

static sint32 get_clut_index(sint16 drawMode, uint16 color)
{
    if ((drawMode & DRAW_COLOR) != COLOR_5)
        return color;
    return VID_NO_CLUT;
}

#define ERASEWRITESTARTLINE 110

void EZ_setErase(sint32 eraseWriteEndLine, uint16 eraseWriteColor)
{
    struct cmdTable* first = (struct cmdTable*)VRAM_ADDR;
    if (eraseWriteEndLine > 0)
    {
        SPR_SetEraseData(eraseWriteColor, 0, 0, 319, eraseWriteEndLine);
    }
    else
        SPR_SetEraseData(eraseWriteColor, 0, 0, 1, 1);

    memset(first, 0, 32);
    first->link = 32 >> 3;
    if (eraseWriteEndLine >= ERASEWRITESTARTLINE)
    {
        first->control = JUMP_ASSIGN | ZOOM_NOPOINT | DIR_NOREV | FUNC_POLYGON;
        first->drawMode = COLOR_5 | ECDSPD_DISABLE;
        first->color = eraseWriteColor;
        first->ax = -160;
        first->ay = ERASEWRITESTARTLINE - 120;
        first->bx = 160;
        first->by = ERASEWRITESTARTLINE - 120;
        first->cx = 160;
        first->cy = eraseWriteEndLine - 120;
        first->dx = -160;
        first->dy = eraseWriteEndLine - 120;
    }
    else
        first->control = SKIP_ASSIGN;
}

void EZ_initSprSystem(sint32 nmCommands, sint32 nmCluts, sint32 nmGour, sint32 eraseWriteEndLine, uint16 eraseWriteColor)
{
    sint32 i;
    uint8* vram;
    SPR_Initial(&vram);
    SPR_SetEosMode(0);
    nmChars = 0;
    for (i = 0; i < MAXNMCHARS; i++)
        chars[i].addr = 0;

    EZ_setErase(eraseWriteEndLine, eraseWriteColor);
    vid_tex_reset();
}

void EZ_setChar(sint32 charNm, sint32 colorMode, sint32 width, sint32 height, uint8* data)
{
    sint32 size;

    assert(charNm >= 0);
    assert(charNm < MAXNMCHARS);
    assert(!(width & 3));

    if (data)
    {
        vid_tex_set(charNm, colorMode, data, width, height);
    }

    size = width * height;
    /* character is not allocated, allocate it */
    if (colorMode >= COLOR_5)
        size <<= 1;
    if (colorMode <= COLOR_1)
        size >>= 1;
    assert(!(((sint32)charStart) & 0x1f));
    if (!chars[charNm].addr)
    {
        chars[charNm].addr = 1;
        chars[charNm].xysize = ((width >> 3) << 8) | height;
    }
    if (data)
    { /* copy char data into area */
        validPtr(data);
        dmaMemCpy(data, (uint8*)((chars[charNm].addr << 3) + VRAM_ADDR), size);
    }
}

void EZ_setLookupTbl(sint32 tblNm, struct sprLookupTbl* tbl)
{
    assert(tblNm >= 0);
    assert(tblNm <= 10);
    validPtr(tbl);
    vid_clut_set(tblNm, (uint16*)tbl, 16);
}

void EZ_openCommand(void)
{
    //
}

void EZ_normSpr(sint16 dir, sint16 drawMode, sint16 color, sint16 charNm, XyInt* pos, struct gourTable* gTable)
{
#if TODO // unused
    struct cmdTable* cmd;
    validPtr(pos);
    cmd = getCmdTable();
    cmd->control = ((ZOOM_NOPOINT | DIR_NOREV | FUNC_NORMALSP) & ~CTRL_DIR) | dir;
    setCharPara(cmd, charNm);
    setDrawPara(cmd, drawMode, color);
    cmd->ax = pos->x;
    cmd->ay = pos->y;
    setGourPara(cmd, gTable);
#endif
    {
        sint32 c[2];
        sint32 xysize = chars[charNm].xysize;
        c[0] = (xysize >> 8) << 3;
        c[1] = (xysize & 0xFF);
        vid_sprite((sint32*)pos, c, color, dir, charNm, get_clut_index(drawMode, color));
    }
}

#if 1
struct slaveDrawResult
{
    XyInt poly[4];
    struct gourTable gtable;
    sint16 tile;
};

void EZ_specialDistSpr(struct slaveDrawResult* sdr, sint32 charNm)
{
#ifdef TODO // unused
    struct cmdTable* cmd;
    cmd = getCmdTable();
    cmd->control = ((ZOOM_NOPOINT | DIR_NOREV | FUNC_DISTORSP) & ~CTRL_DIR);
    setCharPara(cmd, charNm);
    setDrawPara(cmd, UCLPIN_ENABLE | COLOR_5 | HSS_ENABLE | ECD_DISABLE | DRAW_GOURAU, 0);
    {
        sint32 i;
        sint32 *from = (sint32*)sdr->poly, *to = (sint32*)&cmd->ax;
        for (i = 4; i; i--)
            *(to++) = *(from++);
    }
    setGourPara(cmd, &sdr->gtable);
#endif
    vid_poly((sint32*)sdr->poly, (uint16*)&sdr->gtable, charNm, VID_NO_CLUT);
}

void EZ_specialDistSpr2(sint16 charNm, XyInt* xy, struct gourTable* gTable)
{
#ifdef TODO // unused
    struct cmdTable* cmd;
    validPtr(xy);
    cmd = getCmdTable();
    cmd->control = ((ZOOM_NOPOINT | DIR_NOREV | FUNC_DISTORSP) & ~CTRL_DIR);
    setCharPara(cmd, charNm);
    setDrawPara(cmd, UCLPIN_ENABLE | COLOR_5 | HSS_ENABLE | ECD_DISABLE | DRAW_GOURAU, 0);
    {
        sint32 i;
        sint16 *from = (sint16*)xy, *to = (sint16*)&cmd->ax;
        for (i = 8; i; i--)
            *(to++) = *(from++);
    }
    setGourPara(cmd, gTable);
#endif
    vid_poly((sint32*)xy, (uint16*)gTable, charNm, VID_NO_CLUT);
}
#endif

void EZ_distSpr(sint16 dir, sint16 drawMode, sint16 color, sint16 charNm, XyInt* xy, struct gourTable* gTable)
{
#ifdef TODO // unused
    struct cmdTable* cmd;
    validPtr(xy);
    cmd = getCmdTable();
    cmd->control = ((ZOOM_NOPOINT | DIR_NOREV | FUNC_DISTORSP) & ~CTRL_DIR) | dir;
    setCharPara(cmd, charNm);
    setDrawPara(cmd, drawMode, color);
    {
        sint32 i;
        sint16 *from = (sint16*)xy, *to = (sint16*)&cmd->ax;
        for (i = 8; i; i--)
            *(to++) = *(from++);
    }
    setGourPara(cmd, gTable);
#endif
    vid_poly((sint32*)xy, (uint16*)gTable, charNm, get_clut_index(drawMode, color));
}

void EZ_cmd(struct cmdTable* inCmd)
{
#ifdef TODO // unused
    struct cmdTable* cmd;
    validPtr(inCmd);
    cmd = getCmdTable();
    qmemcpy(cmd, inCmd, sizeof(struct cmdTable));
#endif
}

void EZ_scaleSpr(sint16 dir, sint16 drawMode, sint16 color, sint16 charNm, XyInt* pos, struct gourTable* gTable)
{
#ifdef TODO // unused
    struct cmdTable* cmd;
    validPtr(pos);
    cmd = getCmdTable();
    cmd->control = ((ZOOM_NOPOINT | DIR_NOREV | FUNC_SCALESP) & ~CTRL_DIR & ~CTRL_ZOOM) | dir;
    setCharPara(cmd, charNm);
    setDrawPara(cmd, drawMode, color);

    if (dir & CTRL_ZOOM) // x, y, w, h
    {
        cmd->ax = pos[0].x;
        cmd->ay = pos[0].y;
        cmd->bx = pos[1].x;
        cmd->by = pos[1].y;
    }
    else // x0, y0, x1, y1
    {
        cmd->ax = pos[0].x;
        cmd->ay = pos[0].y;
        cmd->cx = pos[1].x;
        cmd->cy = pos[1].y;
    }

    setGourPara(cmd, gTable);
#endif
    if (dir & CTRL_ZOOM) // x, y, w, h
    {
        vid_sprite((sint32*)(pos + 0), (sint32*)(pos + 1), color, dir, charNm, get_clut_index(drawMode, color));
    }
    else // x0, y0, x1, y1
    {
        XyInt p;
        p.x = pos[1].x - pos[0].x;
        p.y = pos[1].y - pos[0].y;
        vid_sprite((sint32*)(pos + 0), (sint32*)&p, color, dir, charNm, get_clut_index(drawMode, color));
    }
}

void EZ_localCoord(sint16 x, sint16 y)
{
#ifdef TODO // unused
    struct cmdTable* cmd;
    cmd = getCmdTable();
    cmd->control = FUNC_LCOORD;
    cmd->ax = x;
    cmd->ay = y;
#endif
    vid_origin(x, y);
}

void EZ_userClip(XyInt* xy)
{
#ifdef TODO // clipping
    struct cmdTable* cmd;
    validPtr(xy);
    cmd = getCmdTable();
    cmd->control = FUNC_UCLIP;
    cmd->ax = xy[0].x;
    cmd->ay = xy[0].y;
    cmd->cx = xy[1].x;
    cmd->cy = xy[1].y;
#endif
}

void EZ_sysClip(void)
{
#ifdef TODO // clipping
    struct cmdTable* cmd;
    cmd = getCmdTable();
    cmd->control = FUNC_SCLIP;
    cmd->cx = 319;
    cmd->cy = 239;
#endif
}

void EZ_polygon(sint16 drawMode, uint16 color, XyInt* xy, struct gourTable* gTable)
{
#ifdef TODO // unused
    struct cmdTable* cmd;
    validPtr(xy);
    cmd = getCmdTable();
    cmd->control = ZOOM_NOPOINT | DIR_NOREV | FUNC_POLYGON;
    setDrawPara(cmd, drawMode, color);
    {
        sint32 i;
        sint16 *from = (sint16*)xy, *to = (sint16*)&cmd->ax;
        for (i = 8; i; i--)
            *(to++) = *(from++);
    }
    setGourPara(cmd, gTable);
#endif
    vid_poly((sint32*)xy, (uint16*)gTable, 0, VID_NO_CLUT);
}

void EZ_polyLine(sint16 drawMode, uint16 color, XyInt* xy, struct gourTable* gTable)
{
#ifdef TODO // polyline
    struct cmdTable* cmd;
    validPtr(xy);
    cmd = getCmdTable();
    cmd->control = ZOOM_NOPOINT | DIR_NOREV | FUNC_POLYLINE;
    setDrawPara(cmd, drawMode, color);
    {
        sint32 i;
        sint16 *from = (sint16*)xy, *to = (sint16*)&cmd->ax;
        for (i = 8; i; i--)
            *(to++) = *(from++);
    }
    setGourPara(cmd, gTable);
#endif
}

void EZ_line(sint16 drawMode, uint16 color, XyInt* xy, struct gourTable* gTable)
{
#ifdef TODO // line
    struct cmdTable* cmd;
    validPtr(xy);
    cmd = getCmdTable();
    cmd->control = ZOOM_NOPOINT | DIR_NOREV | FUNC_LINE;
    setDrawPara(cmd, drawMode, color);

    cmd->ax = xy[0].x;
    cmd->ay = xy[0].y;
    cmd->bx = xy[1].x;
    cmd->by = xy[1].y;

    setGourPara(cmd, gTable);
#endif
}

sint32 EZ_charNoToVram(sint32 charNm)
{
    assert(0);
    return chars[charNm].addr;
}

void EZ_closeCommand(void)
{
#ifdef TODO // unused
#if BUFFERWRITES
    if (cmdBufferUsed > 0)
        flushCmdBuffer();
    if (gourBufferUsed > 0)
        flushGourBuffer();
#endif
    /* make first command link to the frame's command table */
    {
        struct cmdTable* first = (struct cmdTable*)VRAM_ADDR;
        first->link = ((sint32)commandStart[bank]) >> 3;
    }
    /* make last command point to end command */
    if (ccommand == commandStart[bank])
    { /* no commands */
        struct cmdTable* last = ((struct cmdTable*)(VRAM_ADDR + ccommand));
        last->control |= SKIP_ASSIGN;
        last->link = 32 >> 3;
    }
    else
    {
        struct cmdTable* last = ((struct cmdTable*)(VRAM_ADDR + ccommand)) - 1;
        /* last=((struct cmdTable *)(VRAM_ADDR+commandStart[bank]))+8; */
        last->control |= JUMP_ASSIGN;
        last->link = 32 >> 3;
    }
#endif
}

void EZ_executeCommand(void)
{
#ifdef TODO // unused
    SPR_WRITE_REG(SPR_W_PTMR, 1);
#endif
}

void EZ_clearCommand(void)
{
#ifdef TODO // unused
    struct cmdTable* last = ((struct cmdTable*)(VRAM_ADDR + commandStart[bank]));
    last->control |= SKIP_ASSIGN;
    last->link = 32 >> 3;
#endif
}

sint32 EZ_getNextCmdNm(void)
{
#ifdef TODO // unused
#if BUFFERWRITES
    sint32 c = (((uint32)ccommand) >> 5) + cmdBufferUsed;
    sint32 maxc = (((uint32)commandStart[bank]) >> 5) + commandAreaSize;
    if (c > maxc - 1)
        c = maxc - 1;
    return c;
#else
    return ((uint32)ccommand) >> 5;
#endif
#endif
    return 0;
}

void EZ_linkCommand(sint32 cmdNm, sint32 mode, sint32 to)
{
#ifdef TODO // unused
    struct cmdTable* cmd;
#if BUFFERWRITES
    /* if command to be linked is still in the command buffer */
    sint32 offs = cmdNm - (((uint32)ccommand) >> 5);
    if (offs >= 0)
    {
        assert(offs < cmdBufferUsed);
        cmdBuffer[offs].control |= mode;
        cmdBuffer[offs].link = to << 2;
        return;
    }
#endif
    cmd = (struct cmdTable*)(VRAM_ADDR + (cmdNm << 5));
    cmd->control |= mode;
    cmd->link = to << 2;
#endif
}

void EZ_clearScreen(void)
{
#ifdef TODO // clear screen
    XyInt pos[4];
    pos[0].x = -160;
    pos[0].y = -120;
    pos[1].x = 160;
    pos[1].y = -120;
    pos[2].x = 160;
    pos[2].y = 120;
    pos[3].x = -160;
    pos[3].y = 120;
    EZ_openCommand();
    EZ_localCoord(160, 120);
    EZ_polygon(ECDSPD_DISABLE | COLOR_5, 0x8000, pos, NULL);
    EZ_closeCommand();
    SPR_WaitDrawEnd();
    SCL_DisplayFrame();
    EZ_openCommand();
    EZ_localCoord(160, 120);
    EZ_polygon(ECDSPD_DISABLE | COLOR_5, 0x8000, pos, NULL);
    EZ_closeCommand();
    SPR_WaitDrawEnd();
    SCL_DisplayFrame();
#endif
}
