#include <machine.h>
#include <libsn.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sega_spr.h>
#include <sega_scl.h>
#include <sega_int.h>
#include <sega_mth.h>
#include <sega_sys.h>
#include <sega_dbg.h>
#include <sega_per.h>
#include <sega_cdc.h>
#include <sega_gfs.h>
#include <sega_snd.h>

#include "v_blank.h"
#include "file.h"
#include "util.h"
#include "spr.h"
#include "plax.h"

#define PLAXPERSCREEN 128
void movePlax(fix32 yaw, fix32 pitch)
{
    sint32 x, y;
    extern SclRotreg* SclRotregBuff;
    x = -(yaw * PLAXPERSCREEN) / F(45);
    while (x < 0)
        x += 256;
    while (x > 256)
        x -= 256;

    y = -(pitch * PLAXPERSCREEN) / F(45) - 100;
    /* y=-(pitch*PLAXPERSCREEN)/F(45)-40; */

#if 0
 /* poke x offsets */
 POKE(SCL_VDP2_VRAM_A0+0x500,F(x));
 POKE_W(SCL_VDP2_VRAM_A0+0x500+0x34,160+x);

 /* poke y offsets */
 POKE(SCL_VDP2_VRAM_A0+0x500+4,F(y));
 POKE_W(SCL_VDP2_VRAM_A0+0x500+0x36,120+y);
#endif

    SclRotregBuff->screenst.x = F(x);
    SclRotregBuff->viewp.x = 160 + x;
    SclRotregBuff->screenst.y = F(y);
    SclRotregBuff->viewp.y = 20;

    if (SclProcess == 0)
        SclProcess = 1;
}

void enablePlax(sint32 setting)
{
    if (setting)
        Scl_s_reg.dispenbl |= 0x10;
    else
        Scl_s_reg.dispenbl &= ~0x10;
}

static uint16 plaxPal[256];

void retryPlaxPal(void)
{
    sint32 i;
    for (i = 0; i < 256; i++)
        POKE_W(SCL_COLRAM_ADDR + ((256 * 7 + i) << 1), plaxPal[i]);
    /* SCL_SetColRam(0,256*7,256,plaxPal); */
}

void plaxOff(void)
{
    Scl_s_reg.dispenbl &= 0xffef;
    if (SclProcess == 0)
        SclProcess = 1;
    /* SclConfig scfg;
       SCL_InitConfigTb(&scfg);
       scfg.dispenbl=OFF;
       SCL_SetConfig(SCL_RBG0, &scfg); */
}

void initPlax(sint32 fd)
{
    SclConfig scfg;
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

#if 0
 (sint32 x,d;
  double f;
  for (x=0;x<320;x++)
     {f=atan((x-160.0)/160.0)*((double)PLAXPERSCREEN)/0.785398;
      if (abs(x-160)>1)
	 {f=f/(x-160);
	  d=f*65536.0;
	  d=d&0x007fffff;
	 }
      else
	 d=66754/*83200*/ /*0x400*/;
      POKE(SCL_VDP2_VRAM_A0+x*4,d);
     }
 }
#endif

    {
        extern SclRotreg* SclRotregBuff;
        SclRotregBuff->k_tab = 0;
        SclRotregBuff->k_delta.y = 1 << 16;
        SclRotregBuff->k_delta.x = 0; /* are these backwards?? */
        SclRotregBuff->matrix_a = 0;
        SclRotregBuff->matrix_b = F(-1);
        SclRotregBuff->matrix_c = 0;
        SclRotregBuff->matrix_d = F(1);
        SclRotregBuff->matrix_e = 0;
        SclRotregBuff->matrix_f = 0;
    }
    if (SclProcess == 0)
        SclProcess = 1;

#if 0

  POKE(SCL_VDP2_VRAM_A0+0x500+0x54,0); /* coeff start address */
  POKE(SCL_VDP2_VRAM_A0+0x500+0x58,0 /*(160)<<16*/); /* line increment */
  POKE(SCL_VDP2_VRAM_A0+0x500+0x5c,(1)<<16); /* dot increment */


  {static sint32 rotMat[6]={0,-1,0,
			    1, 0,0};
   for (i=0;i<6;i++)
      POKE(SCL_VDP2_VRAM_A0+0x500+0x1c+i*4,F(rotMat[i]));
  }
#endif
}
