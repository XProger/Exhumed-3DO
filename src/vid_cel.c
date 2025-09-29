#ifdef GAPI_CEL

#include "app.h"
#include "vid.h"

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

void vid_init(void)
{
    OpenGraphicsFolio();
    CreateBasicDisplay(&context, DI_TYPE_DEFAULT, 2);

    irqVBL = GetVBLIOReq();
    irqVRAM = GetVRAMIOReq();
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

    con.gc_PenX = 0;
    con.gc_PenY = 0;
    sprintf(buf, "%d", (int)fps);
    DrawText8(&con, context.sc_BitmapItems[context.sc_CurrentScreen], (uint8*)buf);

    DisplayScreen(context.sc_ScreenItems[context.sc_CurrentScreen], NULL);
    context.sc_CurrentScreen ^= 1;
}

void vid_origin(sint32 x, sint32 y)
{
    //
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
    //
}

void vid_poly(sint32 *points, uint16 *colors, sint32 tex_index, sint32 clut_index)
{
    //
}

#endif
