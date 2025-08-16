#define MAXNMPICS 10

typedef struct pic16
{int vramAddress;
 short xsize,ysize; 
} Pic16;

Pic16 overPics[MAXNMPICS];
static int nmPics;
static int vramUsed;

void initOverPics(void)
{vramUsed=1024*180;
 nmPics=0;
}

int loadPic(unsigned char *picData,unsigned short *pallete)
{int xsize,ysize;
 int x,y;
 xsize=*(int *)picData;
 ysize=*(((int *)picData)+1);
 overPics[nmPics].vramAddress=vramUsed;
 overPics[nmPics].xsize=(xsize+7)&(~7);
 overPics[nmPics].ysize=ysize;
 
 picData+=4;
 for (y=0;y<ysize;y++)
    {for (x=0;x<xsize;x++)
	{POKE_W(VRAMSTART+vramUsed,pallete[(int)*picData]);
	 picData++;
	 vramUsed+=2;
	}
     for (x;x<(xsize+7)&(~7);x++)
	{POKE_W(VRAMSTART+vramUsed,0);
	 vramUsed+=2;
	}
    } 
 assert(vramUsed<512*1024);
 return nmPics++;
}


int loadPic(unsigned char *picData,unsigned short *pallete)
{int xsize,ysize;
 int x,y;
 xsize=*(int *)picData;
 ysize=*(((int *)picData)+1);
 overPics[nmPics].vramAddress=vramUsed;
 overPics[nmPics].xsize=(xsize+7)&(~7);
 overPics[nmPics].ysize=ysize;
 
 picData+=4;
 for (y=0;y<ysize;y++)
    {for (x=0;x<xsize;x++)
	{POKE_W(VRAMSTART+vramUsed,pallete[(int)*picData]);
	 picData++;
	 vramUsed+=2;
	}
     for (x;x<(xsize+7)&(~7);x++)
	{POKE_W(VRAMSTART+vramUsed,0);
	 vramUsed+=2;
	}
    } 
 assert(vramUsed<512*1024);
 return nmPics++;
}


void plotOverPic(int x,int y,int index)
{unsigned short cmd[16];
 cmd[0]=0;
 cmd[1]=0;
 cmd[2]=COLOR_5|ECD_DISABLE|SPD_DISABLE;
 cmd[3]=0;
 assert(!(overPic[index].vramAddress & 7));
 cmd[4]=overPic[index].vramAddress>>3;
 assert(!(overPic[index].xsize & 7));
 cmd[5]=(overPic[index].xsize<<5)|item[i].h;
 cmd[6]=x;
 cmd[7]=y;
 cmd[8]=0; cmd[9]=0; cmd[10]=0; cmd[11]=0; cmd[12]=0; cmd[13]=0;
 cmd[14]=0; cmd[15]=0;
 SPR_2Cmd(0,(SprSpCmd *)cmd);
}
