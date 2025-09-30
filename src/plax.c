#include "app.h"
#include "mth.h"
#include "v_blank.h"
#include "file.h"
#include "util.h"
#include "spr.h"
#include "plax.h"

#define PLAXPERSCREEN 128
void movePlax(fix32 yaw, fix32 pitch)
{
    //
}

void enablePlax(sint32 setting)
{
    //
}

static uint16 plaxPal[256];

void retryPlaxPal(void)
{
#ifdef TODO // plax
    sint32 i;
    for (i = 0; i < 256; i++)
        POKE_W(SCL_COLRAM_ADDR + ((256 * 7 + i) << 1), plaxPal[i]);
#endif
    /* SCL_SetColRam(0,256*7,256,plaxPal); */
}

void plaxOff(void)
{
    //
}

void initPlax(sint32 fd)
{
    //SclConfig scfg;
    sint32 x;//, i;
    fs_read(fd, (sint8*)(plaxPal), 256 * 2);
#ifdef TODO // PLAX pal
    for (i = 0; i < 256; i++)
        POKE_W(SCL_COLRAM_ADDR + ((256 * 7 + i) << 1), plaxPal[i]);
#endif
    /* SCL_SetColRam(0,256*7,256,plaxPal); */

    fs_read(fd, (sint8*)&x, 4);

    x = FS_INT(&x);

    assert(x == 512);
    fs_read(fd, (sint8*)&x, 4);

    x = FS_INT(&x);

    assert(x == 256);
#ifdef TODO // plax
    fs_read(fd, (sint8*)SCL_VDP2_VRAM_A1, 512 * 256);

    SCL_InitRotateTable(SCL_VDP2_VRAM_A0 + 0x500, 1, SCL_RBG0, SCL_NON);

    SCL_InitConfigTb(&scfg);
    scfg.dispenbl = 1;
    scfg.bmpsize = SCL_BMP_SIZE_512X256;
    scfg.coltype = SCL_COL_TYPE_256;
    scfg.datatype = SCL_BITMAP;
    scfg.mapover = SCL_OVER_0;
    scfg.plate_addr[0] = 128 * 1024;
    scfg.patnamecontrl = 0;
    SCL_SetConfig(SCL_RBG0, &scfg);

    SCL_SET_R0CAOS(7);
    Scl_r_reg.k_contrl = 0x1;
    Scl_r_reg.k_offset = 0;
    Scl_s_reg.dispenbl |= 0x1000; /* turn off transparency for plax */
    if (SclProcess == 0)
        SclProcess = 1;

    fs_read(fd, (sint8*)SCL_VDP2_VRAM_A0, 320 * 4);
#else
    fs_skip(fd, 512 * 256 + 320 * 4);
#endif
}
