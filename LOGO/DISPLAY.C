#include <machine.h>
#define _SPR2_
#define _SPR3_


#include <libsn.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sega_spr.h>
#include <sega_scl.h>
#include <sega_int.h>
#include <sega_mth.h>
#include <sega_sys.h>
#include <sega_mem.h>
#include <sega_dbg.h>
#include <sega_per.h>
#include <sega_cdc.h>
#include <sega_snd.h>

#include "set_vb.h"
#include "v_blank.h"
#include "file.h"

#define F(a)    ((a) << 16)
#define f(a)  ((a) >> 16)

#define COMMAND_MAX     2048
#define GOUR_TBL_MAX    1024
#define LOOKUP_TBL_MAX  1
#define CHAR_MAX        512
#define DRAW_PRTY_MAX   8192


SPR_2DefineWork( work2d, COMMAND_MAX, GOUR_TBL_MAX, LOOKUP_TBL_MAX, CHAR_MAX,
		DRAW_PRTY_MAX)


void setVDP2(void)
{static Uint16 cycle[]=
   {0xffff, 0xffff,
    0xffff, 0xffff,
    0xffff, 0xffff,
    0xffff, 0xffff};
 SclVramConfig vcfg;
 SCL_Vdp2Init();
 SCL_InitVramConfigTb(&vcfg);
 vcfg.vramModeA=OFF;
 vcfg.vramModeB=OFF;
 vcfg.vramA0=SCL_NON;
 vcfg.vramB0=SCL_NON;
 SCL_SetVramConfig(&vcfg);

 SCL_SetDisplayMode(SCL_NON_INTER,SCL_240LINE,SCL_NORMAL_A);
 SCL_SetColRamMode(SCL_CRM15_2048);

 SCL_SetPriority( SCL_SP0 | SCL_SP1 | SCL_SP2 | SCL_SP3 | SCL_SP4 |
                  SCL_SP5 | SCL_SP6 | SCL_SP7, 7);
 SCL_SetSpriteMode( SCL_TYPE1, SCL_MIX, SCL_SP_WINDOW);
 SCL_SetCycleTable(cycle);
}

short nmFaces;
short nmFrames;
short nmPoints;

typedef struct
{short p[4];
 short owner;
 short pad;
} Face;

typedef struct 
{short p[3];
 short pad;
} Point;

Face *faces;
char *deltas;
Point *points;
Point *startPoints;

void readAnim(char *filename)
{int fd;
 fd=fs_open(filename);
 fs_read(fd,(char *)&nmFaces,2);
 fs_read(fd,(char *)&nmFrames,2);
 fs_read(fd,(char *)&nmPoints,2);
 faces=MEM_Malloc(sizeof(Face)*nmFaces);
 fs_read(fd,(char *)faces,sizeof(Face)*nmFaces);

 startPoints=MEM_Malloc(sizeof(Point)*nmPoints);
 points=MEM_Malloc(sizeof(Point)*nmPoints);
 fs_read(fd,(char *)startPoints,sizeof(Point)*nmPoints);

 deltas=MEM_Malloc(3*nmPoints*nmFrames+nmFrames*((nmFaces+1)/2));
 fs_read(fd,(char *)deltas,3*nmPoints*nmFrames+nmFrames*((nmFaces+1)/2));

 fs_close(fd);
}

static char colorTables[9][3]=
{{0,-8,-8}, /* red */
    {-8,0,-8}, /* green */
    {-8,-8,0}, /* blue */
    {0,-8,-8}, /* red */
    {-8,0,-8}, /* green */
    {-8,-8,0}, /* blue */
    {0,-8,-8}, /* red */
    {-8,0,-8}, /* green */
    {-8,-8,0}, /* blue */
   };

void main(void)
{int frame,i,j,dpos;
 int odd;
 int switchEnable=1;
 XyInt coord[4];

 MEM_Init(0x200000, 0x100000);
 set_imask(0);
 setVDP2();
 SetVblank();

 SPR_2Initial( &work2d );
 SPR_2FrameChgIntr(1);
 SPR_2FrameEraseData(0x8000);
 SPR_SetEraseData(0x8000,0,0,320,240);
 SPR_SetTvMode(SPR_TV_NORMAL,SPR_TV_320X240,OFF);
 readAnim("c:\\borlandc\\pro\\spring\\anim.anm");


 frame=0;
 dpos=0;
 while (1)
    {if (lastInputSample.data & PER_DGT_S)
	{if (switchEnable)
	    {switchEnable=0;
	     MEM_Init(0x200000,0x100000);
	     readAnim("c:\\sr3\\logo\\anim.anm");
	     frame=0;
	    }
	}
     else
	switchEnable=1;

     if (frame>=nmFrames)
	frame=0;
     if (frame==0)
	{for (i=0;i<nmPoints;i++)
	    for (j=0;j<3;j++)
	       points[i].p[j]=startPoints[i].p[j];
	 dpos=0;
	}

     /* apply deltas */
     for (i=0;i<nmPoints;i++)
	for (j=0;j<3;j++)
	   points[i].p[j]+=deltas[dpos++];

     SPR_2OpenCommand(SPR_2DRAW_PRTY_ON);
     coord[0].x = 320 - 1;
     coord[0].y = 240 - 1;
     SPR_2SysClip(0,coord);
     coord[0].x = 320 / 2;
     coord[0].y = 240 / 2;
     SPR_2LocalCoord(1,coord);

	 
     /* draw faces */
     odd=0;
     for (i=0;i<nmFaces;i++)
	{int midz;
	 midz=0;
	 for (j=0;j<4;j++)
	    {coord[j].x=points[faces[i].p[j]].p[0];
	     coord[j].y=points[faces[i].p[j]].p[1];
	     midz+=points[faces[i].p[j]].p[2];
	    }
	 midz>>=2;
	 
	 {int color;
	  if (!odd)
	     {color=deltas[dpos]&0x0f;
	      odd=1;
	     }
	  else
	     {color=((unsigned char)deltas[dpos])>>4;
	      odd=0;
	      dpos++;
	     }
	  if ((coord[1].x-coord[0].x)*(coord[2].y-coord[0].y)-
	      (coord[1].y-coord[0].y)*(coord[2].x-coord[0].x)>0)
	     continue;

	  color+=16;
	  {int r,g,b;
	   r=color+colorTables[faces[i].owner][0];
	   g=color+colorTables[faces[i].owner][1];
	   b=color+colorTables[faces[i].owner][2];
	   color=0x8000|(b<<10)|(g<<5)|r;
	  }
	  SPR_2Polygon(8192-midz,COMPO_REP|COLOR_5,color,coord,NO_GOUR);
	 }
	}
     if (odd)
	dpos++;
     
     frame++;
     SPR_2CloseCommand();
     SCL_DisplayFrame();
    }
}




