#include "app.h"
#include "v_blank.h"
#include "util.h"

sint32 fadeDir, fadePos, fadeEnd;

#define INPUTQSIZE 16 /* dont change this */
uint16 inputQ[INPUTQSIZE];
uint16 lastInputSample;
uint16 inputAccum;
sint16 analogX;
sint16 analogY;
sint16 analogTR;
sint16 analogTL;
sint8 controlerPresent;
sint8 analogControlerPresent;
sint8 analogIndexButtonsPresent;
sint8 inputQHead, inputQTail;

/*
uint16	PadData1  = 0x0000;
uint16	PadData1E = 0x0000;
uint16	PadData2  = 0x0000;
uint16	PadData2E = 0x0000;
*/
uint32 vtimer = 0, htimer = 0, secs = 0;

void UsrHblankIn(void)
{
    htimer++;
}

void processInput(void)
{
    uint16 accum;
    sint32 i, pos, size, id;
    accum = 0xffff;

#ifdef TODO // input controlerPresent
    controlerPresent = 0;
#else
    controlerPresent = 1;
#endif
    analogControlerPresent = 0;
    analogIndexButtonsPresent = 0;

    accum = app_input();

    lastInputSample = accum;
    inputAccum &= accum;
    /* add input sample */
    inputQ[(sint32)inputQHead] = accum;
    inputQHead = (inputQHead + 1) & 0xf;
}

sint8 enableDoorReset = 1;

void UsrVblankStart(void)
{
    if (fadeDir)
    {
        fadePos += fadeDir;
        if (fadeDir > 0 && fadePos > fadeEnd)
        {
            fadePos = fadeEnd;
            fadeDir = 0;
        }
        if (fadeDir < 0 && fadePos < fadeEnd)
        {
            fadePos = fadeEnd;
            fadeDir = 0;
        }
#if 0
     POKE_W(SCL_VDP2_VRAM+0x180110,0x7f);/*enable color offset for all planes*/
     POKE_W(SCL_VDP2_VRAM+0x180112,0x00);/*use offset reg 0 for all planes*/
#endif

#ifdef TODO // fade wtf?
        POKE_W(SCL_VDP2_VRAM + 0x180114, fadePos & 0x1ff);
        POKE_W(SCL_VDP2_VRAM + 0x180116, fadePos & 0x1ff);
        POKE_W(SCL_VDP2_VRAM + 0x180118, fadePos & 0x1ff);
#endif
    }
}

void initInput(void)
{
    inputQHead = 0;
    inputAccum = 0xffff;
    lastInputSample = 0xffff;
}

void UsrVblankEnd(void)
{
    vtimer++;
    processInput();
}

sint32 perFlag;

void CheckVblankEnd(void)
{
    perFlag = 0;
}

#ifndef NDEBUG

sint32 errorQ[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
sint32 prQ[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
sint32 errorQSize = 0;

#pragma interrupt
void userBreakBlam(void)
{
#ifdef TODO // remove
    sint32 dummy[1];
    __asm__ volatile("sts.l pr,%0\n" : "=r"((sint32)dummy[0]));
    errorQ[errorQSize] = dummy[4];
    prQ[errorQSize] = dummy[0];
    errorQSize = (errorQSize + 1) & 0xf;

    fadePos = 255;
    fadeDir = -10;
    fadeEnd = 0;

    return;
#endif
#if 0
 sint32 i,x,y;
 for (y=0;y<256;y++)
    for (x=0;x<512;x++)
       {POKE_W(SCL_VDP2_VRAM+(y<<10)+(x<<1),RGB(x&0xf,x&0xf,x&0xf));
       }
 POKE_W(SCL_VDP2_VRAM+0x180110,0x7f);/*enable color offset for all planes*/
 POKE_W(SCL_VDP2_VRAM+0x180112,0x00);/*use offset reg 0 for all planes*/
 POKE_W(SCL_VDP2_VRAM+0x180114,0); /* zero the color offset */
 POKE_W(SCL_VDP2_VRAM+0x180116,0);
 POKE_W(SCL_VDP2_VRAM+0x180118,0);
 POKE_W(SCL_VDP2_VRAM+0x180020,1); /* turn nbg0 on */
 POKE_W(SCL_VDP2_VRAM+0x180028,0x32); /* set 16 bit color & on bitmap */
 POKE_W(SCL_VDP2_VRAM+0x180040,0x0);
 POKE_W(SCL_VDP2_VRAM+0x180042,0x0);
 POKE_W(SCL_VDP2_VRAM+0x18003c,0x0);
 POKE_W(SCL_VDP2_VRAM+0x18003e,0x0);

 POKE_W(SCL_VDP2_VRAM+0x18003e,0x0);

 for (i=0;i<8;i++)
    POKE(SCL_VDP2_VRAM+(i<<4)+0x180010,0x4444);
 while (1);
 while (1)
    for (i=-255;i<255;i++)
       {POKE_W(SCL_VDP2_VRAM+0x180114,i & 0x1ff);
	POKE_W(SCL_VDP2_VRAM+0x180116,i & 0x1ff);
	POKE_W(SCL_VDP2_VRAM+0x180118,i & 0x1ff);
       }
#endif
}
#endif

void SetVblank(void)
{ /* V_Blank Out */
#ifdef TODO // render vblank set
    fadeDir = 0;
    fadePos = 0;
    fadeEnd = 0;

    INT_ChgMsk(INT_MSK_NULL, INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
    INT_SetScuFunc(INT_SCU_VBLK_OUT, CheckVblankEnd);
    INT_ChgMsk(INT_MSK_VBLK_OUT, INT_MSK_NULL);

    perFlag = 1;
    while (perFlag)
        ;

    PER_LInit(PER_KD_PERTIM, 6, PER_SIZE_NCON_15, PadWorkArea, 0);

    /* V-Blank */
    INT_ChgMsk(INT_MSK_NULL, INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT | INT_MSK_HBLK_IN);
#ifdef STATUSTEXT
    INT_SetScuFunc(INT_SCU_HBLK_IN, UsrHblankIn);
#endif
    INT_SetScuFunc(INT_SCU_VBLK_IN, UsrVblankStart);
    INT_SetScuFunc(INT_SCU_VBLK_OUT, UsrVblankEnd);
#ifdef STATUSTEXT
    INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT | INT_MSK_HBLK_IN, INT_MSK_NULL);
#else
    INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT, INT_MSK_NULL);
#endif

#if 1
#ifndef NDEBUG
    /* set user break interrupt */
    /* asm("trapa #34");*/
    POKE_W(0xffffff48, 0); /* break bus cycle register */
    /* INT_SetScuFunc(12,userBreakBlam); */
    POKE(0x06000000 + 0x30, userBreakBlam);
#if 1
    POKE(0xffffff40, 0x00000000); /* break address */
    POKE(0xffffff44, 0x00000fff); /* break address mask */
    POKE_W(0xffffff48, 0x06c); /* break bus cycle register */
#endif
#endif
#endif
#endif
}
