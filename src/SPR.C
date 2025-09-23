#include <machine.h>

#include <sega_mth.h>
#include <sega_scl.h>
#include <sega_spr.h>
#include <string.h>

#include "util.h"
#include "spr.h"
#include "dma.h"

#define BUFFERWRITES 1

static sint32 bank;
static uint8 *commandStart[2], *clutStart, *gourStart[2], *charStart;
static uint8 *ccommand, *cgouraud;

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

#if BUFFERWRITES
static sint32 commandAreaSize, gourauAreaSize;
static sint32 cmdBufferUsed, totCommand;
static sint32 gourBufferUsed, totGourau;
static struct gourTable gourBuffer[GOURBUFFERSIZE];
static struct cmdTable cmdBuffer[CMDBUFFERSIZE];
#endif

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
    commandStart[0] = (uint8*)64;
    commandStart[1] = commandStart[0] + (nmCommands << 5);
    clutStart = commandStart[1] + (nmCommands << 5);
    gourStart[0] = clutStart + (nmCluts << 5);
    gourStart[1] = gourStart[0] + (nmGour << 3);
    charStart = gourStart[1] + (nmGour << 3);
    nmChars = 0;
    for (i = 0; i < MAXNMCHARS; i++)
        chars[i].addr = 0;
    bank = 0;

    EZ_setErase(eraseWriteEndLine, eraseWriteColor);
#if BUFFERWRITES
    cmdBufferUsed = 0;
    gourBufferUsed = 0;
    commandAreaSize = nmCommands;
    gourauAreaSize = nmGour;
#endif

    {
        struct cmdTable* end = ((struct cmdTable*)VRAM_ADDR) + 1;
        memset(end, 0, 32);
        end->control = CTRL_END;
    }
}

void EZ_setChar(sint32 charNm, sint32 colorMode, sint32 width, sint32 height, uint8* data)
{
    sint32 size = width * height;
    assert(charNm >= 0);
    assert(charNm < MAXNMCHARS);
    assert(!(width & 3));
    /* character is not allocated, allocate it */
    if (colorMode >= COLOR_5)
        size <<= 1;
    if (colorMode <= COLOR_1)
        size >>= 1;
    assert(!(((sint32)charStart) & 0x1f));
    if (!chars[charNm].addr)
    {
        chars[charNm].addr = ((sint32)charStart) >> 3;
        charStart += size;
        charStart = (uint8*)((((sint32)charStart) + 31) & (~0x1f));
        assert(((sint32)charStart) < 1024 * 512);
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
    dmaMemCpy(tbl, (uint8*)(VRAM_ADDR + clutStart + (tblNm << 5)), 1 << 5);
}

void EZ_openCommand(void)
{
    bank = !bank;
    ccommand = commandStart[bank];
    cgouraud = gourStart[bank];
    totCommand = 0;
    totGourau = 0;
#if BUFFERWRITES
    cmdBufferUsed = 0;
    gourBufferUsed = 0;
#endif
}

#if BUFFERWRITES
static void flushCmdBuffer(void)
{
    if (cmdBufferUsed + totCommand > commandAreaSize)
        cmdBufferUsed = commandAreaSize - totCommand;
    dmaMemCpy(cmdBuffer, ccommand + VRAM_ADDR, cmdBufferUsed << 5);
    ccommand += cmdBufferUsed << 5;
    totCommand += cmdBufferUsed;
    cmdBufferUsed = 0;
}

static void flushGourBuffer(void)
{
    if (gourBufferUsed + totGourau > gourauAreaSize)
        gourBufferUsed = gourauAreaSize - totGourau;
    dmaMemCpy(gourBuffer, cgouraud + VRAM_ADDR, gourBufferUsed << 3);
    cgouraud += gourBufferUsed << 3;
    totGourau += gourBufferUsed;
    gourBufferUsed = 0;
}

static inline struct cmdTable* getCmdTable(void)
{
    if (cmdBufferUsed == CMDBUFFERSIZE)
        flushCmdBuffer();
    return cmdBuffer + (cmdBufferUsed++);
}

static inline void setGourPara(struct cmdTable* cmd, struct gourTable* gTable)
{
    if (!gTable)
        cmd->grshAddr = 0;
    else
    {
        validPtr(gTable);
        if (gourBufferUsed == GOURBUFFERSIZE)
            flushGourBuffer();
        gourBuffer[gourBufferUsed] = *gTable;
        cmd->grshAddr = (((sint32)cgouraud) >> 3) + gourBufferUsed++;
    }
}
#else
static inline struct cmdTable* getCmdTable(void)
{
    struct cmdTable* ret = (struct cmdTable*)(ccommand + VRAM_ADDR);
    ccommand += 32;
    return ret;
}

static inline void setGourPara(struct cmdTable* cmd, struct gourTable* gTable)
{
    if (!gTable)
        cmd->grshAddr = 0;
    else
    {
        struct gourTable* g = (struct gourTable*)(cgouraud + VRAM_ADDR);
        validPtr(gTable);
        *g = *gTable;
        cmd->grshAddr = (((sint32)cgouraud) >> 3);
        cgouraud += 8;
    }
}
#endif

static inline void setCharPara(struct cmdTable* cmd, sint16 charNm)
{
    validPtr(cmd);
    cmd->charAddr = chars[charNm].addr;
    cmd->charSize = chars[charNm].xysize;
}

static inline void setDrawPara(struct cmdTable* cmd, sint16 drawMode, sint16 color)
{
    validPtr(cmd);
    cmd->drawMode = drawMode;
    if ((drawMode & DRAW_COLOR) == COLOR_1)
        cmd->color = (((sint32)clutStart) + (color << 5)) >> 3;
    else
        cmd->color = color;
}

void EZ_normSpr(sint16 dir, sint16 drawMode, sint16 color, sint16 charNm, XyInt* pos, struct gourTable* gTable)
{
    struct cmdTable* cmd;
    validPtr(pos);
    cmd = getCmdTable();
    cmd->control = ((ZOOM_NOPOINT | DIR_NOREV | FUNC_NORMALSP) & ~CTRL_DIR) | dir;
    setCharPara(cmd, charNm);
    setDrawPara(cmd, drawMode, color);
    cmd->ax = pos->x;
    cmd->ay = pos->y;
    setGourPara(cmd, gTable);
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
}
void EZ_specialDistSpr2(sint16 charNm, XyInt* xy, struct gourTable* gTable)
{
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
}
#endif

void EZ_distSpr(sint16 dir, sint16 drawMode, sint16 color, sint16 charNm, XyInt* xy, struct gourTable* gTable)
{
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
}

void EZ_cmd(struct cmdTable* inCmd)
{
    struct cmdTable* cmd;
    validPtr(inCmd);
    cmd = getCmdTable();
    qmemcpy(cmd, inCmd, sizeof(struct cmdTable));
}

void EZ_scaleSpr(sint16 dir, sint16 drawMode, sint16 color, sint16 charNm, XyInt* pos, struct gourTable* gTable)
{
    struct cmdTable* cmd;
    validPtr(pos);
    cmd = getCmdTable();
    cmd->control = ((ZOOM_NOPOINT | DIR_NOREV | FUNC_SCALESP) & ~CTRL_DIR & ~CTRL_ZOOM) | dir;
    setCharPara(cmd, charNm);
    setDrawPara(cmd, drawMode, color);

    if (dir & CTRL_ZOOM)
    {
        cmd->ax = pos[0].x;
        cmd->ay = pos[0].y;
        cmd->bx = pos[1].x;
        cmd->by = pos[1].y;
    }
    else
    {
        cmd->ax = pos[0].x;
        cmd->ay = pos[0].y;
        cmd->cx = pos[1].x;
        cmd->cy = pos[1].y;
    }

    setGourPara(cmd, gTable);
}

void EZ_localCoord(sint16 x, sint16 y)
{
    struct cmdTable* cmd;
    cmd = getCmdTable();
    cmd->control = FUNC_LCOORD;
    cmd->ax = x;
    cmd->ay = y;
}

void EZ_userClip(XyInt* xy)
{
    struct cmdTable* cmd;
    validPtr(xy);
    cmd = getCmdTable();
    cmd->control = FUNC_UCLIP;
    cmd->ax = xy[0].x;
    cmd->ay = xy[0].y;
    cmd->cx = xy[1].x;
    cmd->cy = xy[1].y;
}

void EZ_sysClip(void)
{
    struct cmdTable* cmd;
    cmd = getCmdTable();
    cmd->control = FUNC_SCLIP;
    cmd->cx = 319;
    cmd->cy = 239;
}

void EZ_polygon(sint16 drawMode, sint16 color, XyInt* xy, struct gourTable* gTable)
{
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
}

void EZ_polyLine(sint16 drawMode, sint16 color, XyInt* xy, struct gourTable* gTable)
{
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
}

void EZ_line(sint16 drawMode, sint16 color, XyInt* xy, struct gourTable* gTable)
{
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
}

sint32 EZ_charNoToVram(sint32 charNm)
{
    return chars[charNm].addr;
}

void EZ_closeCommand(void)
{
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
}

void EZ_executeCommand(void)
{
    SPR_WRITE_REG(SPR_W_PTMR, 1);
}

void EZ_clearCommand(void)
{
    struct cmdTable* last = ((struct cmdTable*)(VRAM_ADDR + commandStart[bank]));
    last->control |= SKIP_ASSIGN;
    last->link = 32 >> 3;
}

sint32 EZ_getNextCmdNm(void)
{
#if BUFFERWRITES
    sint32 c = (((uint32)ccommand) >> 5) + cmdBufferUsed;
    sint32 maxc = (((uint32)commandStart[bank]) >> 5) + commandAreaSize;
    if (c > maxc - 1)
        c = maxc - 1;
    return c;
#else
    return ((uint32)ccommand) >> 5;
#endif
}

void EZ_linkCommand(sint32 cmdNm, sint32 mode, sint32 to)
{
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
}

void EZ_clearScreen(void)
{
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
}
