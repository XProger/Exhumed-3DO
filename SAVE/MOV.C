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
#include "local.h"
#include "sound.h"
#include "dma.h"

#define ENDCODE 65535

static void setupVDP2(void)
{static Uint16 cycle[]=
    {0x4444, 0xeeee,
     0x4444, 0xeeee,
     0x5555, 0xeeee,
     0x5555, 0xeeee};
 SclConfig scfg;
 SclVramConfig vcfg;
 SCL_InitVramConfigTb(&vcfg);
 vcfg.vramModeA=OFF;
 vcfg.vramModeB=OFF;
 vcfg.vramA0=SCL_NON;
 vcfg.vramB0=SCL_NON;
 SCL_SetVramConfig(&vcfg);
 SCL_SetCycleTable(cycle);

 /* setup VDP2Sprite screen */
 SCL_InitConfigTb(&scfg);
 scfg.dispenbl=ON;
 scfg.bmpsize=SCL_BMP_SIZE_512X256;
 scfg.coltype=SCL_COL_TYPE_32K;
 scfg.datatype=SCL_BITMAP;
 scfg.mapover=SCL_OVER_0;
 scfg.plate_addr[0]=0;
 scfg.patnamecontrl=0;
 SCL_SetConfig(SCL_NBG0, &scfg);
 scfg.plate_addr[0]=256*1024;
 SCL_SetConfig(SCL_NBG1, &scfg);
 SCL_SetPriority(SCL_NBG0,2);
 SCL_SetPriority(SCL_NBG1,1);
}

#define MAXNMBUFFERS 1000
static int qPos,qHead,qTail,nmBuffers;
static unsigned char **buffer;

int sndRingPos,BPS;
static int ourPrtn;

static GfsHn cdFile; /* only one cd file may be open at a time */

static int maybeReadBuffer(void)
{Sint32 size;
 int nextHead;
 GFS_NwExecOne(cdFile);
 assert(qHead<nmBuffers);
 nextHead=qHead+1;
 if (nextHead==nmBuffers)
    nextHead=0;
 if (nextHead==qTail)
    return 0;

 CDC_GetSctNum(ourPrtn,&size);
 if (size>0)
    {/* suck down another buffer full */
     GFS_Fread(cdFile,1,buffer[qHead],2048);
     qHead=nextHead;
     return 1;
    }
 return 0;
}

static unsigned char deQByte(void)
{unsigned char r=*(buffer[qTail]+qPos);
 assert(qPos<2048);
 assert(qTail<nmBuffers);
 qPos++;
 if (qPos>=2048)
    {qTail++;
     if (qTail==nmBuffers)
	qTail=0;
     qPos=0;
    }
 return r;
}

static unsigned short deQCount(void)
{int r;
 r=deQByte()<<8;
 r+=deQByte();
 return r;
}

static void deQCopy(void *dest,int size)
{int s;
 assert(qPos<2048);
 while (size)
    {s=2048-qPos;
     if (s>size)
	{/* qmemcpy(dest,buffer[qTail]+qPos,size); */
	 dmaMemCpy(buffer[qTail]+qPos,dest,size);
	 qPos+=size;
	 break;
	}
     /* qmemcpy(dest,buffer[qTail]+qPos,s); */
     dmaMemCpy(buffer[qTail]+qPos,dest,s);
     dest+=s;
     size-=s;
     assert(qTail<nmBuffers);
     qTail++;
     if (qTail==nmBuffers)
	qTail=0;
     qPos=0;
    }
}

#define SNDBASE 0x05a00000
static void deQSound(void)
{int amount;
 int s;
 amount=deQByte()<<8;
 amount+=deQByte();
 while (amount)
    {s=65536-sndRingPos;
     if (s>amount)
	{if (BPS==16)
	    deQCopy((void *)(SNDBASE+(sndRingPos<<1)),amount<<1);
	 else
	    deQCopy((void *)(SNDBASE+(sndRingPos)),amount);
	 sndRingPos+=amount;
	 return;
	}
     if (BPS==16)
	deQCopy((void *)(SNDBASE+(sndRingPos<<1)),s<<1);
     else
	deQCopy((void *)(SNDBASE+(sndRingPos)),s);
     sndRingPos=0;
     amount-=s;
    }
}

#define FRAMESKIP 3

void playMovie(void)
{unsigned char *booffer[MAXNMBUFFERS];
 unsigned short black[300];
 short nmFrames,frame;
 int waveRate;
 int i,size,screenPos,s;

 doItAgain:
 buffer=booffer; /* naf */
 mem_init();
 initSound();
 /* allocate buffers */
 for (i=0;i<300;i++)
    black[i]=RGB(0,0,0);

 for (nmBuffers=0;nmBuffers<MAXNMBUFFERS;nmBuffers++)
    {buffer[nmBuffers]=mem_nocheck_malloc(!(nmBuffers&1),2048);
     if (!buffer[nmBuffers])
	break;
    }

 dPrint("Movie allocated %d buffers.\n",nmBuffers);
 qHead=0; qTail=0; qPos=0;

 {int id=GFS_NameToId("UNCOMP.MOV");
  assert(id>=0);
  cdFile=GFS_Open(id);
  assert(cdFile);
  GFS_NwCdRead(cdFile,1500*2048); /* start prefetch */
  GFS_SetTransPara(cdFile,10);
 }

 for (i=0;i<nmBuffers-1;i++)
    GFS_Fread(cdFile,1,buffer[i],2048);
 qHead=nmBuffers-1;
 ourPrtn=CDC_CdGetLastBuf((Sint32 *)&ourPrtn);

 for (i=0;i<512*1024;i+=2)
    POKE_W(SCL_VDP2_VRAM+i,0x8000);
 setupVDP2();


 EZ_initSprSystem(100,8,100,
		  0,0x8000);
 i=initFonts(0);
 initPicSystem(i,((int []){50,0,0,0,-1}));
 SCL_SetDisplayMode(SCL_NON_INTER,SCL_240LINE,SCL_NORMAL_A);
 SCL_SetFrameInterval(FRAMESKIP);
 SPR_SetTvMode(SPR_TV_NORMAL,SPR_TV_320X240,OFF);
 SPR_SetEraseData(0x0000,0,0,319,239);

 POKE_W(SCL_VDP2_VRAM+0x180020,0x0f03); /* transparency and enables */

 /* read movie header */
 nmFrames=*(short *)(buffer[qTail]+6);
 dPrint("Movie has %d frames\n",nmFrames);

 BPS=*(short *)(buffer[qTail]+8);
 waveRate=*(unsigned short *)(buffer[qTail]+10);

 qPos=12;
 qTail=0;

 sndRingPos=0;
 deQSound();

 if (BPS)
    {/* start sound */
     int base=SNDBASE+0x100000;
     POKE_W(base+2,0); /* start addr */
     POKE_W(base+6,0xffff); /* end addr */
     POKE_W(base+0x10,waveRate);
     POKE_W(base+0x16,(7<<13)|(0<<8)); /* vol and pan */
     POKE_W(base+4,0); /* loop start */
     if (BPS==16)
	POKE_W(base,0x1800|(1<<5)); /* kon, ex and loop */
     else
	POKE_W(base,0x1800|(1<<5)|0x10); /* kon, ex and loop */
    }

 for (frame=0;frame<nmFrames;frame++)
    {/* decode next frame in buffer */
     vtimer=0;
     screenPos=0;
     while (1)
	{/* decode unchanged area */
	 size=deQCount();
	 if (size==ENDCODE)
	    break;
	 while (size>=320)
	    {size-=320;
	     screenPos+=512;
	    }
	 if (size+(screenPos & 0x1ff)>=320)
	    screenPos+=size+512-320;
	 else
	    screenPos+=size;

	 /* decode changed area */
	 size=deQCount();
	 if (size==ENDCODE)
	    break;
	 while (size>0)
	    {/* write out data up to next line end */
	     s=320-(screenPos & 0x1ff);
	     assert(s>0);
	     if (size<s)
		{/* we won't get to the end of the line */
		 deQCopy((void *)(SCL_VDP2_VRAM+(screenPos<<1)),size<<1);
		 screenPos+=size;
		 break;
		}
	     deQCopy((void *)(SCL_VDP2_VRAM+(screenPos<<1)),s<<1);
	     size-=s;
	     screenPos=(screenPos+512)&(~511);
	    }
	}
     deQSound();

#if 1
     {XyInt parms[4];
      int fullBuffers;
      fullBuffers=qHead-qTail;
      if (fullBuffers<0)
	 fullBuffers+=nmBuffers;
      parms[0].x=0; parms[0].y=220;
      parms[1].x=0; parms[1].y=230;
      parms[2].x=fullBuffers>>2; parms[2].y=230;
      parms[3].x=fullBuffers>>2; parms[3].y=220;
      EZ_openCommand();
      EZ_polygon(ECD_DISABLE|SPD_DISABLE,RGB(0,0,31),
		 parms,NULL);
      EZ_closeCommand();
     }
#endif
     PEEK_W(SCL_VDP2_VRAM+0x180002);
     while ((vtimer<FRAMESKIP-1 ||
	     (0x3ff & PEEK_W(SCL_VDP2_VRAM+0x18000a))<200) &&
	    maybeReadBuffer())
	PEEK_W(SCL_VDP2_VRAM+0x180002);
     SCL_DisplayFrame();
    }

 for (i=0;i<512*1024;i+=2)
    POKE_W(SCL_VDP2_VRAM+i,0x8000);

 GFS_NwStop(cdFile); /* stop prefetch */
 GFS_Close(cdFile);
 goto doItAgain;
}
