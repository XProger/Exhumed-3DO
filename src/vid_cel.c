#ifdef GAPI_CEL

#include "app.h"
#include "vid.h"

#include <displayutils.h>
#include <operamath.h>
#include <io.h>
#include <mem.h>
#include <graphics.h>

ScreenContext context;

Item irqVBL;
Item irqVRAM;

sint32 fps_frame;
sint32 fps_time;
sint32 fps;

IOInfo clearInfo = {
    FLASHWRITE_CMD, IO_QUICK, 0, 0, -1, 0, 0,
    { NULL, 0 },
    { NULL, 320 * 240 * 2 }
};

typedef struct Face
{
    uint32 ccb_Flags;

    struct Face* ccb_NextPtr;
    const void* ccb_SourcePtr;
    const void* ccb_PLUTPtr;

    sint32 ccb_XPos;
    sint32 ccb_YPos;
    sint32 ccb_HDX;
    sint32 ccb_HDY;
    sint32 ccb_VDX;
    sint32 ccb_VDY;
    sint32 ccb_HDDX;
    sint32 ccb_HDDY;
    sint32 ccb_PIXC;
} Face;

#define MAX_FACES 1024

static Face faces[MAX_FACES];
static Face *face;

static sint32 vid_cx, vid_cy;

#define TEX_SHIFT   4
#define TEX_SIZE    (1 << TEX_SHIFT)

#define RGB555(r, g, b) (b >> 3) | ((g >> 3) << 5) | ((r >> 3) << 10)

const uint16 test_tex4_plut[16] = {
    RGB555(41, 41, 255),
    RGB555(255, 255, 255),
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint8 test_tex4[4 + 4 + TEX_SIZE * TEX_SIZE / 2];

void gen_tex4(void)
{
    sint32 x, y;
    uint32 *pre;
    uint32 woffset, pitch;
    uint8 *data = test_tex4;

    pre = (uint32*)data;
    woffset = TEX_SIZE >> 3; // 4 bpp
    if (woffset < 2)
    {
        woffset = 2;
    }
    pitch = woffset << 2; // in bytes
    pre[0] = ((TEX_SIZE - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) | PRE0_BPP_4 | PRE0_BGND;
    pre[1] = ((TEX_SIZE - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT) | PRE1_TLLSB_PDC0 | ((woffset - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET8_SHIFT);
    data += 8;

    memset(data, 0, TEX_SIZE * pitch);

    for (y = 0; y < TEX_SIZE; y++)
    {
        data[pitch * y] = 1 | (1 << 4);
        data[pitch * y + (TEX_SIZE / 2 - 1)] = 1 | (1 << 4);
    }

    for (x = 0; x < TEX_SIZE / 2; x++)
    {
        data[x] = data[x + pitch * (TEX_SIZE - 1)] = 1 | (1 << 4);
    }
}

void vid_init(void)
{
    OpenGraphicsFolio();
    CreateBasicDisplay(&context, DI_TYPE_DEFAULT, 2);

    irqVBL = GetVBLIOReq();
    irqVRAM = GetVRAMIOReq();

    gen_tex4();

    face = faces;
}

void vid_resize(sint32 width, sint32 height)
{
    //
}

void vid_clear(void)
{
    WaitVBL(irqVBL, 1);
    clearInfo.ioi_Recv.iob_Buffer = context.sc_Bitmaps[context.sc_CurrentScreen]->bm_Buffer;
    SendIO(irqVRAM, &clearInfo);

    {
        sint32 time = app_time();
        fps_frame++;
        if (time >= fps_time)
        {
            fps = fps_frame;
            fps_frame = 0;
            fps_time = time + 1000;
        }
    }
}

void vid_blit(void)
{
    char buf[32];
    GrafCon con;

    if (face > faces)
    {
        face--;
        face->ccb_Flags |= CCB_LAST;
        DrawScreenCels(context.sc_ScreenItems[context.sc_CurrentScreen], (CCB*)faces);
        face = faces;
    }

    con.gc_PenX = 0;
    con.gc_PenY = 0;
    sprintf(buf, "%d", (int)fps);
    DrawText8(&con, context.sc_BitmapItems[context.sc_CurrentScreen], (uint8*)buf);

    DisplayScreen(context.sc_ScreenItems[context.sc_CurrentScreen], NULL);
    context.sc_CurrentScreen ^= 1;
}

void vid_origin(sint32 x, sint32 y)
{
    vid_cx = x;
    vid_cy = y;
}

void vid_tex_reset(void)
{
    //
}

void vid_tex_set(sint32 tex_index, sint32 format, const void *data, sint32 width, sint32 height)
{
    //
}

void vid_clut_set(sint32 clut_index, const uint16 *data, sint32 length)
{
    //
}

void vid_sprite(sint32 *a, sint32 *c, uint16 color, sint32 dir, sint32 tex_index, sint32 clut_index)
{
    face->ccb_NextPtr = face + 1;
    face->ccb_Flags =
        CCB_NPABS  |
        CCB_SPABS  |
        CCB_PPABS  |
        CCB_LDSIZE |
        CCB_LDPRS  |
        CCB_LDPPMP |
        CCB_LDPLUT |
        CCB_YOXY   |
        CCB_ACW    | CCB_ACCW |
        CCB_ACE    |
        CCB_BGND;

    face->ccb_XPos = (vid_cx + a[0]) << 16;
    face->ccb_YPos = (vid_cy + a[1]) << 16;
    face->ccb_HDX = c[0] << (20 - TEX_SHIFT);
    face->ccb_HDY = 0;
    face->ccb_VDX = 0;
    face->ccb_VDY = c[1] << (16 - TEX_SHIFT);
    face->ccb_HDDX = 0;
    face->ccb_HDDY = 0;
    face->ccb_PIXC = PPMPC_MF_8 | PPMPC_SF_8;
    face->ccb_SourcePtr = test_tex4;
    face->ccb_PLUTPtr = test_tex4_plut;

    face++;
}

void vid_poly(sint32 *points, uint16 *colors, sint32 tex_index, sint32 clut_index)
{
    sint32 x0 = points[0 * 2 + 0];
    sint32 y0 = points[0 * 2 + 1];
    sint32 x1 = points[1 * 2 + 0];
    sint32 y1 = points[1 * 2 + 1];
    sint32 x2 = points[2 * 2 + 0];
    sint32 y2 = points[2 * 2 + 1];
    sint32 x3 = points[3 * 2 + 0];
    sint32 y3 = points[3 * 2 + 1];

    sint32 hdx0 = (x1 - x0) << (20 - TEX_SHIFT);
    sint32 hdy0 = (y1 - y0) << (20 - TEX_SHIFT);
    sint32 hdx1 = (x2 - x3) << (20 - TEX_SHIFT);
    sint32 hdy1 = (y2 - y3) << (20 - TEX_SHIFT);
    sint32 vdx0 = (x3 - x0) << (16 - TEX_SHIFT);
    sint32 vdy0 = (y3 - y0) << (16 - TEX_SHIFT);
    sint32 hddx = (hdx1 - hdx0) >> TEX_SHIFT;
    sint32 hddy = (hdy1 - hdy0) >> TEX_SHIFT;

    face->ccb_NextPtr = face + 1;
    face->ccb_Flags =
        CCB_NPABS  |
        CCB_SPABS  |
        CCB_PPABS  |
        CCB_LDSIZE |
        CCB_LDPRS  |
        CCB_LDPPMP |
        CCB_LDPLUT |
        CCB_YOXY   |
        CCB_ACW    | CCB_ACCW |
        CCB_ACE    |
        CCB_BGND;

    face->ccb_XPos = (vid_cx + x0) << 16;
    face->ccb_YPos = (vid_cy + y0) << 16;
    face->ccb_HDX = hdx0;
    face->ccb_HDY = hdy0;
    face->ccb_VDX = vdx0;
    face->ccb_VDY = vdy0;
    face->ccb_HDDX = hddx;
    face->ccb_HDDY = hddy;
    face->ccb_PIXC = PPMPC_MF_8 | PPMPC_SF_8;
    face->ccb_SourcePtr = test_tex4;
    face->ccb_PLUTPtr = test_tex4_plut;

    face++;
}

#endif
