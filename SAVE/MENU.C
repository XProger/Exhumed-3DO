#include <machine.h>
#include <sega_spr.h>
#include <sega_scl.h>
#include <string.h>

#include "menu.h"
#include "print.h"
#include "v_blank.h"
#include "util.h"
#include "spr.h"
#include "pic.h"
#include "file.h"
#include "local.h"
#include "sound.h"

#include "weapon.h"
#include "gamestat.h"

#define BUTTONBASECOLOR RGB(12,12,31)
#define BUTTONHICOLOR RGB(25,25,31)
#define BUTTONLOCOLOR RGB(5,5,21)

enum {IT_RECT,IT_TEXT,IT_BUTTON,IT_PICTURE,IT_FONTTEXT,IT_WAVYBUTTON};

#define DLGFONT 1
#define MAXDLGSIZE 40
#define VRAMSTART (VRAM_ADDR)
DlgItem dlgItem[MAXDLGSIZE];

unsigned short lastUsedSelButton;
static int nmItems;
static int currentButton;

#define MAXNMPICS 40

static unsigned short *picPals[MAXNMPICS];
static unsigned int *picDatas[MAXNMPICS];
static int picVram[MAXNMPICS];
static int vramUsed;

static int texHeight,texWidth,texVramPos,texX,texY;
static int fontHeight;

void dlg_selectButton(int b)
{currentButton=b;
}

void dlg_centerStuff(void)
{int i;
 for (i=0;i<nmItems;i++)
    {switch (dlgItem[i].type)
	{case IT_WAVYBUTTON:
	    dlgItem[i].xp=F(-(getStringWidth(2,dlgItem[i].text)/2));
	    break;
	   }    
    }
}

void initOverPics(void)
{vramUsed=1024*256;
}

void loadOverPic(int picNm)
{int xsize,ysize;
 int x,y;
 unsigned char *picData=(unsigned char *)picDatas[picNm];
 unsigned short *pallete=picPals[picNm];
 xsize=*(int *)picData;
 ysize=*(((int *)picData)+1);
 picVram[picNm]=vramUsed;
 
 picData+=8;
 for (y=0;y<ysize;y++)
    {for (x=0;x<xsize;x++)
	{POKE_W(VRAMSTART+vramUsed,pallete[(int)*picData]);
	 picData++;
	 vramUsed+=2;
	}
    } 
 assert(vramUsed<512*1024);
}


typedef struct 
{int x1,y1,x2,y2,width,type;
} BevelData;

int loadOverBase(unsigned char *picData,unsigned short *pallete,int xsize,
		 int ysize,BevelData *bevels,int nmBevels)
{int txstart,tx,ty,x1,y1,width,height;
 int vramBase=vramUsed;

 width=*(int *)picData;
 height=*(((int *)picData)+1);
 txstart=MTH_GetRand() % (width-1);
 ty=MTH_GetRand() % (height-1);
 txstart=0;
 ty=0;
 for (y1=0;y1<ysize;y1++)
    {tx=txstart;
     for (x1=0;x1<xsize;x1++)
	{int bevel=0;
	 int lineSide[4];
	 int b;
	 unsigned short c;
	 for (b=0;b<nmBevels;b++)
	    {if (x1<bevels[b].x1 || x1>=bevels[b].x2 ||
		 y1<bevels[b].y1 || y1>=bevels[b].y2)
		continue;
	     if (bevels[b].type==0 &&
		 x1>bevels[b].x1+bevels[b].width &&
		 y1>bevels[b].y1+bevels[b].width &&
		 x1<bevels[b].x2-bevels[b].width &&
		 y1<bevels[b].y2-bevels[b].width)
		continue;
		 
		 
	     lineSide[0]=(x1-bevels[b].x1)>(y1-bevels[b].y1);
	     lineSide[1]=-(y1-bevels[b].y1)>(x1-bevels[b].x2);
	     lineSide[2]=(x1-bevels[b].x2)>(y1-bevels[b].y2);
	     lineSide[3]=-(y1-bevels[b].y2)>(x1-bevels[b].x1);
	     
	     if (y1<bevels[b].y1+bevels[b].width && lineSide[0] && lineSide[1])
		bevel=1;
	     if (x1>=bevels[b].x2-bevels[b].width && !lineSide[1] && 
		 lineSide[2])
		bevel=2;
	     if (y1>=bevels[b].y2-bevels[b].width && !lineSide[2] && 
		 !lineSide[3])
		bevel=3;
	     if (x1<bevels[b].x1+bevels[b].width && lineSide[3] && 
		 !lineSide[0])
		bevel=4;
	     if (bevels[b].type==1)
		if (bevel)
		   bevel=((bevel+1)&3)+1;
		else
		   bevel=5;
	     if (bevel)
		break;
	    }
	 
	 c=pallete[picData[tx+ty*width+8]];
	 if (bevel)
	    {int r,g,b,off;
	     
	     r=c & 0x1f;
	     g=(c>>5) & 0x1f;
	     b=(c>>10) & 0x1f;
	     
	     off=0;
	     if (bevel==2)
		off=-5;
	     if (bevel==3)
		off=-6;
	     if (bevel==1)
		off=6;
	     if (bevel==4)
		off=5;
	     if (bevel==5)
		off=-3;
	     
	     r+=off; 
	     g+=off; 
	     b+=off; 
	     if (b<0) b=0;
	     if (g<0) g=0;
	     if (r<0) r=0;
	     if (b>31) b=31;
	     if (g>31) g=31;
	     if (r>31) r=31;
	     c=RGB(r,g,b);
	    }
	 POKE_W(VRAMSTART+vramUsed,c); 
	 vramUsed+=2;
	 assert(vramUsed<512*1024);
	 if (++tx>=width)
	    tx=0;
	}
     if (++ty>=height)
	ty=0;
    }
 return vramBase;
}


void plotOverPicW(int x,int y,int w,int h,int vram,int drawWord)
{unsigned short cmd[16];
 cmd[0]=0;
 cmd[1]=0;
 cmd[2]=drawWord;
 cmd[3]=0;
 assert(!(vram & 7));
 cmd[4]=vram>>3;
 assert(!(w & 7));
 cmd[5]=(w<<5) | h;
 cmd[6]=x;
 cmd[7]=y;
 cmd[8]=0; cmd[9]=0; cmd[10]=0; cmd[11]=0; cmd[12]=0; cmd[13]=0;
 cmd[14]=0; cmd[15]=0;
 EZ_cmd((struct cmdTable *)cmd);
}

void plotOverPic(int x,int y,int picNm)
{plotOverPicW(x,y,*(int *)picDatas[picNm],*(((int *)picDatas[picNm])+1),
	      picVram[picNm],COLOR_5|ECD_DISABLE);
}

void plotOverPicShadow(int x,int y,int picNm)
{plotOverPicW(x,y,*(int *)picDatas[picNm],*(((int *)picDatas[picNm])+1),
	      picVram[picNm],COLOR_5|ECD_DISABLE|COMPO_SHADOW);
}

void dlg_clear(void)
{nmItems=0;
 currentButton=-1;
 texVramPos=-1;
}

void dlg_init(int fd)
{loadPicSet(fd,picPals,picDatas,MAXNMPICS);
 mem_lock();
 dlg_clear();
}

void dlg_addBase(int x,int y,int w,int h,BevelData *bevel,int nmBevels)
{texX=x;
 texY=y;
 texWidth=w;
 texHeight=h;
 texVramPos=loadOverBase((char *)(picDatas[0]),picPals[0],w,h,bevel,
			 nmBevels);
}

void dlg_addRect(int x,int y,int w,int h,int color)
{dlgItem[nmItems].xp=F(x);
 dlgItem[nmItems].yp=F(y);
 dlgItem[nmItems].w=w;
 dlgItem[nmItems].h=h;
 dlgItem[nmItems].color=color;
 dlgItem[nmItems].type=IT_RECT;
 nmItems++;
}

void dlg_addFontText(int x,int y,int w,int font,char *text)
{dlgItem[nmItems].xp=F(x);
 dlgItem[nmItems].yp=F(y);
 dlgItem[nmItems].w=w;
 dlgItem[nmItems].type=IT_FONTTEXT;
 dlgItem[nmItems].text=text;
 dlgItem[nmItems].color=font;
 nmItems++;
}

void dlg_addText(int x,int y,int w,char *text)
{dlgItem[nmItems].xp=F(x);
 dlgItem[nmItems].yp=F(y);
 dlgItem[nmItems].w=w;
 dlgItem[nmItems].type=IT_TEXT;
 dlgItem[nmItems].text=text;
 nmItems++;
}

void dlg_addCenteredText(int x,int y,int w,char *text)
{dlg_addText(x+(w-getStringWidth(DLGFONT,text))/2,y,w,text);
}

void dlg_addButton(int pressCode,int x,int y,int w,int h,char *text)
{dlgItem[nmItems].xp=F(x);
 dlgItem[nmItems].yp=F(y);
 dlgItem[nmItems].w=w;
 dlgItem[nmItems].h=h;
 dlgItem[nmItems].code=pressCode;
 
 dlgItem[nmItems].type=IT_BUTTON;
 dlgItem[nmItems].text=text;
 if (currentButton==-1)
    currentButton=nmItems;
 nmItems++;
}

void dlg_addBigWavyButton(int pressCode,int x,int y,char *text)
{dlgItem[nmItems].xp=F(x);
 dlgItem[nmItems].yp=F(y);
 dlgItem[nmItems].code=pressCode;
 dlgItem[nmItems].w=0;
 dlgItem[nmItems].h=0;

 dlgItem[nmItems].type=IT_WAVYBUTTON;
 dlgItem[nmItems].text=text;
 if (currentButton==-1)
    currentButton=nmItems;
 nmItems++;
}

/* returns height of draw text */
enum drawMode {DM_NORM,DM_SHADOW,DM_NODRAW};
static int drawCenteredText(int x,int y,int width,char *text,int drawMode,
			    int dlgFont)
{int x1,y1,lineWidth,j;
 char *lstart,*lend,*p,*c;
 lstart=text;
 lend=text-1;
 lineWidth=0;
 y1=y;
 for (p=text;;p++)
    {if (*p==' ' || *p==0 || *p=='\n')
	{j=lineWidth;
	 for (c=lend+1;c<p;c++)
	    j+=getCharWidth(dlgFont,*c)+1;
	 if (j<width)
	    {/* add this unit to the line */
	     lend=p-1;
	     lineWidth=j;
	     if (*p==' ')
		continue;
	    }
	 /* otherwise flush out the line */
	 x1=x+((width-lineWidth)>>1);
	 for (c=lstart;c<=lend;c++)
	    {switch (drawMode)
		{case DM_SHADOW:
		    drawCharShadow(x1,y1,dlgFont,*c);
		    break;
		 case DM_NORM:
		    drawChar(x1,y1,dlgFont,*c);
		    break;
		   }
	     x1+=getCharWidth(dlgFont,*c)+1;
	    }
	 y1+=fontHeight+2;
	 lstart=lend+1;
/*	 lend=p;*/
	 lineWidth=0;
	 if (!*p)
	    return y1-y;
	}
    }
}


static void drawText(int x,int y,int width,char *text,int shadow,int dlgFont)
{int rside=x+width;
 int x1,y1,j;
 char *w,*p,*chr;
 /* w points to beginning of next word */
 x1=x; y1=y;
 w=text;
 for (p=text;;p++)
    {if (*p==' ' || *p==0)
	{/* time to draw the next word.  Decide if it should be on this line
	    or the next */
	 j=x1-1;
	 for (chr=w;chr<p;chr++)
	    j+=getCharWidth(dlgFont,*chr)+1;

	 if (j>rside)
	    {x1=x;
	     y1+=fontHeight+1;
	    }
	 while (w<p)
	    {if (shadow)
		drawCharShadow(x1,y1,dlgFont,*w);
	     else
		drawChar(x1,y1,dlgFont,*w);
	     x1+=getCharWidth(dlgFont,*w)+1;
	     w++;
	    }
	 x1+=getCharWidth(dlgFont,' ')+1;
	 w++;
	}
     if (*p==0)
	break;
    }
}


static void dlg_draw(int currentButton,int pressed)
{int i;
 XyInt p[5];
 for (i=0;i<nmItems;i++)
    {switch (dlgItem[i].type)
	{
#if 0
      case IT_PICTURE:
	    {unsigned short cmd[16];
	     cmd[0]=0;
	     cmd[1]=0;
	     cmd[2]=COLOR_5|ECD_DISABLE|SPD_DISABLE;
	     cmd[3]=0;
	     assert(!(dlgItem[i].color & 7));
	     cmd[4]=dlgItem[i].color>>3;
	     assert(!(dlgItem[i].w & 7));
	     cmd[5]=(dlgItem[i].w<<5)|dlgItem[i].h;
	     cmd[6]=dlgItem[i].x;
	     cmd[7]=dlgItem[i].y;
	     cmd[8]=0; cmd[9]=0; cmd[10]=0; cmd[11]=0; cmd[12]=0; cmd[13]=0;
	     cmd[14]=0; cmd[15]=0;
	     EZ_cmd(0,(struct cmdTable *)cmd);
	     break;
	    }
#endif
         case IT_RECT:
	    {int x=f(dlgItem[i].xp);
	     int y=f(dlgItem[i].yp);
	     p[0].x=x; p[0].y=y;
	     p[1].x=x+dlgItem[i].w; p[1].y=y;
	     p[2].x=x+dlgItem[i].w; p[2].y=y+dlgItem[i].h;
	     p[3].x=x; p[3].y=y+dlgItem[i].h;
	     EZ_polygon(ECD_DISABLE|SPD_DISABLE|COMPO_REP,
			dlgItem[i].color,p,NULL);
	     break;
	    }
	 case IT_BUTTON:
	    {int drawWord=COMPO_REP|ECD_DISABLE|SPD_DISABLE;
	     int x=f(dlgItem[i].xp);
	     int y=f(dlgItem[i].yp);
	     if (i!=currentButton)
		drawWord|=COMPO_HARF;
	     p[0].x=x; p[0].y=y;
	     p[1].x=x+dlgItem[i].w; p[1].y=y;
	     p[2].x=x+dlgItem[i].w; p[2].y=y+dlgItem[i].h;
	     p[3].x=x; p[3].y=y+dlgItem[i].h;
	     EZ_polygon(drawWord,BUTTONBASECOLOR,p,NULL);
	     EZ_polyLine(drawWord,0x8000,p,NULL); 
	     p[0].x++; p[0].y++;
	     p[1].x--; p[1].y++;
	     p[2].x--; p[2].y--;
	     p[3].x++; p[3].y--;
	     p[4]=p[0];
	     if (i!=currentButton || !pressed)
		{EZ_line(drawWord,BUTTONHICOLOR,p,NULL);
		 EZ_line(drawWord,BUTTONLOCOLOR,p+1,NULL);
		 EZ_line(drawWord,BUTTONLOCOLOR,p+2,NULL);
		 EZ_line(drawWord,BUTTONHICOLOR,p+3,NULL);
		}
	     else
		{EZ_polyLine(drawWord,0x8000,p,NULL);
		 p[0].x++; p[0].y++;
		 p[1].x--; p[1].y++;
		 p[2].x--; p[2].y--;
		 p[3].x++; p[3].y--;
		 EZ_polyLine(drawWord,BUTTONLOCOLOR,p,NULL); 
		}
		 
	     x=f(dlgItem[i].xp)+dlgItem[i].w/2;
	     y=f(dlgItem[i].yp)+dlgItem[i].h/2;
	     y-=fontHeight/2;
	     x-=getStringWidth(DLGFONT,dlgItem[i].text)/2;
	     drawText(x,y,dlgItem[i].w,dlgItem[i].text,0,DLGFONT);
	     break;
	    }
	 case IT_TEXT:
	    {int x=f(dlgItem[i].xp);
	     int y=f(dlgItem[i].yp);
	     drawCenteredText(x+1,y+1,dlgItem[i].w,dlgItem[i].text,DM_SHADOW,
			      DLGFONT);
	     drawCenteredText(x+1,y+1,dlgItem[i].w,dlgItem[i].text,DM_SHADOW,
			      DLGFONT);
	     drawCenteredText(x,y,dlgItem[i].w,dlgItem[i].text,DM_NORM,
			      DLGFONT);
	     break;
	    }
	 case IT_FONTTEXT:
	    {int x=f(dlgItem[i].xp);
	     int y=f(dlgItem[i].yp);
	     drawText(x+1,y+1,dlgItem[i].w,dlgItem[i].text,1,
		      dlgItem[i].color);
	     drawText(x+1,y+1,dlgItem[i].w,dlgItem[i].text,1,
		      dlgItem[i].color);
	     drawText(x,y,dlgItem[i].w,dlgItem[i].text,0,
		      dlgItem[i].color);
	     break;
	    }
	 case IT_WAVYBUTTON:
	    {static Fixed32 waveCycle=0;
	     if (i==currentButton)
		{int color=MTH_Sin(waveCycle)>>13;
		 waveCycle+=F(8);
		 if (waveCycle>F(180))
		    waveCycle-=F(360);
		 drawStringGouro(f(dlgItem[i].xp),f(dlgItem[i].yp),2,
				 greyTable[15+color],greyTable[15-color],
				 dlgItem[i].text);
		}
	     else
		drawString(f(dlgItem[i].xp),f(dlgItem[i].yp),2,dlgItem[i].text);	    
	    }
	   }
    }
}

static int moveSel(int cb,int dx,int dy,int movement)
{int nowx,nowy;
 int d,i;
 int bestButton,bestDist;
 int ccx,ccy,icx,icy;
 nowx=f(dlgItem[cb].xp);
 nowy=f(dlgItem[cb].yp);
 bestButton=-1;
 bestDist=0x7fffffff;
 ccx=f(dlgItem[cb].xp)+(dlgItem[cb].w>>1);
 ccy=f(dlgItem[cb].yp)+(dlgItem[cb].h>>1);
 for (i=0;i<nmItems;i++)
    {if (dlgItem[i].type!=IT_BUTTON &&
	 dlgItem[i].type!=IT_WAVYBUTTON)
	continue;
     icx=f(dlgItem[i].xp)+(dlgItem[i].w>>1);
     icy=f(dlgItem[i].yp)+(dlgItem[i].h>>1);
     d=(icx-ccx)*dx+
       (icy-ccy)*dy;
     if (d<=0)
	continue;
     d=0;
     if (movement==MENUMOVE_FREE)
	{d+=abs(icx-ccx);
	 d+=abs(icy-ccy);
	 if (icx*dy==ccx*dy &&
	     icy*dx==ccy*dx)
	    d-=10000;
	}
     if (movement==MENUMOVE_HORZ)
	{d+=abs(icx-ccx);
	 if (icx*dy==ccx*dy)
	    d-=10000;
	}
     if (movement==MENUMOVE_VERT)
	{d+=abs(icy-ccy);
	 if (icy*dx==ccy*dx)
	    d-=10000;
	}
     if (d<bestDist)
	{bestButton=i;
	 bestDist=d;
	}
    }
 if (bestButton!=-1)
    return bestButton;
 return cb;
}

#define SELKEYS (PER_DGT_A|PER_DGT_B|PER_DGT_C|PER_DGT_X|PER_DGT_Y|PER_DGT_Z|PER_DGT_S|PER_DGT_TL|PER_DGT_TR)

int dlg_run(int selSound,int pushSound,int movement)
{int data,lastData,changeData,i;
#if 0
 int first=2;
#endif
 int pressed=0;
 int returnTime=0;

 fontHeight=getFontHeight(DLGFONT);
 data=lastInputSample;
 SCL_SetFrameInterval(0xfffe);

 while (1)
    {EZ_openCommand();
     EZ_sysClip();
     EZ_localCoord(320/2,240/2);

     if (texVramPos>=0)
	plotOverPicW(texX,texY,texWidth,texHeight,texVramPos,
		     COLOR_5|ECD_DISABLE);
     dlg_draw(currentButton,pressed); 
     SPR_WaitDrawEnd();
     EZ_closeCommand();
#if 0
     if (first>=0)
	{if (!first)
	    SCL_SetColOffset(SCL_OFFSET_A,SCL_SP0|SCL_NBG0,
			     0,0,0);
	 first--;
	}
#endif
     SCL_DisplayFrame();
     if (returnTime)
	{if (returnTime==1)
	    {resetPics();
	     return dlgItem[currentButton].code;
	    }
	 returnTime--;
	 continue;
	}

     if (currentButton==-1)
	{if (!returnTime)
	    returnTime=2;
	 continue;
	}
     lastData=data;
     data=lastInputSample;
     changeData=lastData ^ data;

     i=currentButton;
     if ((changeData & PER_DGT_D) && !(data & PER_DGT_D))
	currentButton=moveSel(currentButton,0,1,movement);
     if ((changeData & PER_DGT_U) && !(data & PER_DGT_U))
	currentButton=moveSel(currentButton,0,-1,movement);
     if ((changeData & PER_DGT_L) && !(data & PER_DGT_L))
	currentButton=moveSel(currentButton,-1,0,movement);
     if ((changeData & PER_DGT_R) && !(data & PER_DGT_R))
	currentButton=moveSel(currentButton,1,0,movement);
     if (i!=currentButton && selSound>=0)
	playSound(0,selSound);	

     if ((changeData&~data)&SELKEYS) 
	{lastUsedSelButton=(changeData&~data) & SELKEYS;
	 if (pushSound>=0)
	    playSound(0,pushSound);	
	 pressed=1;
	}
     
     if ((data & SELKEYS)==SELKEYS && pressed)
	{returnTime=5;
	 pressed=0;
	}
    }
}

void dlg_runMessage(char *message,int w,int h)
{int bw=80;
 int bh=20;
 static BevelData bevel[2]=
    {{0,0,0,0,5,0},
     {9,9,0,0,2,1}};
 w=(w+7)&(~7);
 bevel[0].x2=w; bevel[0].y2=h;
 bevel[1].x2=w-9; bevel[1].y2=h-40;
 initOverPics();
 dlg_clear();
 dlg_addBase(-w/2,-h/2,w,h,bevel,2);

 dlg_addText(-w/2+15,-h/2+15,w-30,message);
 dlg_addButton(1,-bw/2,h/2-bh-13,bw,bh,"OK");
 dlg_run(-1,-1,MENUMOVE_FREE);
}

void dlg_flashMessage(char *message1,char *message2,int w,int h)
{BevelData bevel;
 w=(w+7)&(~7);
 bevel.x1=0; bevel.y1=0;
 bevel.x2=w; bevel.y2=h;
 bevel.width=5; bevel.type=0;
 initOverPics();
 dlg_clear();
 dlg_addBase(-w/2,-h/2,w,h,&bevel,1);
 dlg_addText(-160,-h/2+15,320,message1);
 dlg_addText(-160,-h/2+25,320,message2);
 dlg_run(-1,-1,MENUMOVE_FREE);
}

#define CGW (26*8)
#define CGH 200
int dlg_runChooseGame(char *heading,char **saveNames,int nmSaveGames)
{static BevelData bevel[1]=
    {{0,0,CGW,CGH,5,0}
    };
 int i;
 initOverPics(); 
 dlg_clear();
 dlg_addBase(-CGW/2,-CGH/2,CGW,CGH,bevel,1);

 dlg_addCenteredText(-CGW/2+10,-CGH/2+10,CGW-20,heading);

 for (i=0;i<nmSaveGames;i++)
    {dlg_addButton(i,
		   -CGW/2+20,-CGH/2+30+i*24,
		   CGW-40,18,
		   saveNames[i]);
    }
 dlg_addButton(-1,
	       -40,CGH/2-28,
	       80,18,getText(LB_PROMPTS,1));

 return dlg_run(-1,-1,MENUMOVE_FREE);
}

#define ESW (29*8)
#define ESH 150
static char nameBuffer[80];
char *dlg_enterString(char *heading)
{int i;
 int slen;
 static BevelData bevel[2]=
    {{30,25,ESW-30,55,2,1},
     {0,0,ESW,ESH,5,0}
    };
 static char *keyboard[]=
    {"Q","W","E","R","T","Y","U","I","O","P","",
     "A","S","D","F","G","H","J","K","L","",
     "Z","X","C","V","B","N","M",""};

 initOverPics();
 dlg_clear();
 dlg_addBase(-ESW/2,-ESH/2,ESW,ESH,bevel,2);

 dlg_addText(-160,-ESH/2+10,320,heading);

 nameBuffer[0]=0;
 slen=0;
 dlg_addFontText(-ESW/2+35,-ESH/2+34,ESW-30,2,nameBuffer);

 {int row;
  const int keyX=16,keyY=12,keySpace=5;
  int x,y;
  int chr,p;
  chr=0;
  for (row=0;row<3;row++)
     {for (p=chr;keyboard[p][0];p++) ;
      x=-((p-chr)*(keyX+keySpace)-keySpace)/2;
      y=row*20-10;
      for (;keyboard[chr][0];chr++)
	 {dlg_addButton(keyboard[chr][0],x,y,keyX,keyY,
			keyboard[chr]);
	  x+=keyX+keySpace;
	 }
      chr++;
     }
  dlg_addButton(1,-79,y+20,70,keyY,"Delete");
  dlg_addButton(2,  9,y+20,70,keyY,"Done");
 }
 do
    {i=dlg_run(-1,-1,MENUMOVE_FREE);
     if (i>='A' && i<='Z' && slen<20)
	{nameBuffer[slen]=i;
	 slen++;
	 nameBuffer[slen]=0;
	}
     if (i==1 && slen>0)
	{slen--;
	 nameBuffer[slen]=0;
	}
    }
 while (i!=2);

 return nameBuffer;
}

int dlg_runYesNo(char *message,int w,int h)
{int bw=80;
 int bh=20;
 int space=20;
 static BevelData bevel[2]=
    {{0,0,0,0,5,0},
     {9,9,0,0,2,1}};

 w=(w+7)&(~7);
 bevel[0].x2=w; bevel[0].y2=h;
 bevel[1].x2=w-9; bevel[1].y2=h-40;
 initOverPics();
 dlg_clear();
 dlg_addBase(-w/2,-h/2,w,h,bevel,2);

 dlg_addText(-w/2+15,-h/2+15,w-30,message);

 dlg_addButton(1,-(2*bw+space)/2,h/2-(bh+13),bw,bh,
	       getText(LB_PROMPTS,3));
 dlg_addButton(0,space/2,h/2-(bh+13),bw,bh,
	       getText(LB_PROMPTS,4));
 return dlg_run(-1,-1,MENUMOVE_FREE);
}

int runTravelQuestion(char *destination)
{char buff[160];
 POKE_W(SCL_VDP2_VRAM+0x180114,-255); /* reset color offsets */
 POKE_W(SCL_VDP2_VRAM+0x180116,-255);
 POKE_W(SCL_VDP2_VRAM+0x180118,-255);
 SCL_SetColOffset(SCL_OFFSET_A,SCL_SP0|SCL_NBG0|SCL_RBG0,
		  -255,-255,-255);
 dontDisplayVDP2Pic();
 EZ_openCommand(); EZ_closeCommand(); SCL_DisplayFrame();
 EZ_openCommand(); EZ_closeCommand(); SCL_DisplayFrame();
 SCL_SetColOffset(SCL_OFFSET_A,SCL_SP0|SCL_NBG0,
		  0,0,0); 
 sprintf(buff,"Travel to\n%s?",destination);
 return dlg_runYesNo(buff,200,80);
}


#define INVWIDTH 29*8
#define INVHEIGHT 155
static BevelData inventoryBevel[]=
{{108,9,INVWIDTH-9,74+9,3,1},
 {9,111,INVWIDTH-9,INVHEIGHT-9,3,1},
 {0,0,INVWIDTH,INVHEIGHT,5,0}};
#define NMINVBUTTONS 5

static int inventoryNum[NMINVBUTTONS]={2,8,6,0,1};
static int inventoryPicStart[NMINVBUTTONS]={28,14,22,0,22};
static int textStart[NMINVBUTTONS]={0,2,10,16,17};

#define NMDUSTMOTES 128

/* returns 1 if quit requested */
void runInventory(int inventory,int keyMask,int *mapState,
		  int fade,int fadeButton,int fadeSelection)
{XyInt p;
 int i,canGoUp,canGoDown;
 int selectedButton;
 int data,lastData,changeData;
 int slidePos[MAXNMPICS];
 int quitEnable=0;
 int frameCount;
 struct soundSlotRegister *tinkleSound;

 XyInt dust[NMDUSTMOTES];
 int dustAge[NMDUSTMOTES];
 int dustHead=0,dustTail=0;

 int fadeReg=0,fadeWidth=0,fadeHeight=0,fadeSize=0,fadePic=0,fadeCount=0;

 checkStack();

 stopAllLoopedSounds();
 POKE_W(SCL_VDP2_VRAM+0x180114,-255); /* reset color offsets */
 POKE_W(SCL_VDP2_VRAM+0x180116,-255);
 POKE_W(SCL_VDP2_VRAM+0x180118,-255);
 SCL_SetColOffset(SCL_OFFSET_A,SCL_SP0|SCL_NBG0,
		  -255,-255,-255);

 for (i=0;i<NMINVBUTTONS;i++)
    slidePos[i]=0;

 slidePos[0]=*mapState;
 slidePos[1]=bitScanForward((inventory>>8)&0xff,-1);
 slidePos[2]=bitScanForward(inventory&0x3f,-1);
 dontDisplayVDP2Pic();
 initOverPics();
 for (i=1;i<30;i++)
    loadOverPic(i);
 
 data=lastInputSample;
 lastData=data;
 SCL_SetFrameInterval(0xfffe);

 dlg_clear();
 dlg_addBase(-INVWIDTH/2,-INVHEIGHT/2,INVWIDTH,INVHEIGHT,inventoryBevel,3);
 selectedButton=0;

 if (fade)
    {selectedButton=fadeButton;
     slidePos[selectedButton]=fadeSelection;
     fadePic=inventoryPicStart[selectedButton]+slidePos[selectedButton];
     fadeReg=1;
     fadeWidth=*(int *)picDatas[fadePic];
     fadeHeight=*(((int *)picDatas[fadePic])+1);
     fadeSize=fadeWidth*fadeHeight;
     fadeCount=0;
     dustHead=dustTail=0;
     for (i=0;i<fadeSize*2;i+=2)
	{POKE_W(VRAMSTART+picVram[fadePic]+i,0);
	}
     {struct soundSlotRegister ssr;
      initSoundRegs(level_staticSoundMap[ST_INTERFACE]+2,0,0,&ssr);
      ssr.reg[4]=(0<<11)+(16<<6)+5; /* dd2r, d1r, and ar */
      ssr.reg[5]=(31<<5)+5; /* dl & rr */
      tinkleSound=playSoundMegaE(54,&ssr);
     }
    }
 
 frameCount=0;
 while (1)
    {EZ_openCommand();
     EZ_sysClip();
     EZ_localCoord(320/2,240/2);

     drawString(-(getStringWidth(1,getText(LB_PROMPTS,2))>>1),
		-89,
		1,getText(LB_PROMPTS,2));
     /* draw background */
     plotOverPicW(texX,texY,texWidth,texHeight,texVramPos,COLOR_5|ECD_DISABLE);
     
     /* buttons */
     for (i=0;i<NMINVBUTTONS;i++)
	if (i==selectedButton)
	   plotOverPic(-105,-68+i*20,2);
	else
	   plotOverPic(-105,-68+i*20,1);
     
     for (i=0;i<NMINVBUTTONS;i++)
	{int x,y;
	 x=-62-getStringWidth(1,getText(LB_INVBUTTONS,i))/2;
	 y=-63+i*20-getFontHeight(1)/2;
	 if (i!=selectedButton)
	    drawString(x,y,1,getText(LB_INVBUTTONS,i));
	 else
	    drawStringGouro(x,y,1,RGB(31,31,15),RGB(31,31,15),
			    getText(LB_INVBUTTONS,i));
	}

     /* key label */
     plotOverPic(-6,12,3);
     drawString(-3,12,1,getText(LB_INVBUTTONS,NMINVBUTTONS));

     /* keys */
     for (i=0;i<4;i++)
	if ((keyMask>>i)&1)
	   plotOverPic(35+19*i,12,i+4+4);
	else
	   plotOverPic(35+19*i,12,i+4);

     /* decide mobility */
     canGoUp=-1;
     canGoDown=-1;
     if (slidePos[selectedButton]!=-1)
	switch (selectedButton)
	   {case 0:
	       if (slidePos[selectedButton]>0) canGoDown=0;
	       if (slidePos[selectedButton]<1) canGoUp=1;
	       break;
	    case 1:
	       canGoDown=bitScanBackwards((inventory>>8)&0xff,
					  slidePos[selectedButton]);
	       canGoUp=bitScanForward((inventory>>8)&0xff,
				      slidePos[selectedButton]);
	       break;
	    case 2:
	       canGoDown=bitScanBackwards(inventory&0x3f,
					  slidePos[selectedButton]);
	       canGoUp=bitScanForward(inventory&0x3f,
				      slidePos[selectedButton]);
	       break;
	      }
     /* arrows */
     if (canGoDown!=-1)
	{plotOverPic(0,-35,12);
	}
     if (canGoUp!=-1)
	{plotOverPic(90,-35,13);
	}

     /* picture window */
     if (slidePos[selectedButton]!=-1 && inventoryNum[selectedButton])
	{int p,px,py;
	 p=inventoryPicStart[selectedButton]+slidePos[selectedButton];
	 px=50-((*(int *)picDatas[p])>>1);
	 py=-33-((*(((int *)picDatas[p])+1))>>1);
	 plotOverPicShadow(px+2,py+2,p);
	 plotOverPic(px,py,p);
	 if (fade)
	    {unsigned short *pallete=picPals[fadePic];
	     XyInt line[2];
	     if (fadeCount>8196/64+40)
		fade=0;
	     if (fadeCount<8196/64)
		{for (i=0;i<64;i++)
		    {if (fadeReg & 1)
			fadeReg=(fadeReg>>1) ^ (0x1b00);
		    else
		       fadeReg=fadeReg>>1;
		     if (fadeReg<fadeSize)
			{unsigned short color=
			    pallete[((unsigned char *)(picDatas[fadePic]))
				    [fadeReg+8]];
			 POKE_W(VRAMSTART+picVram[fadePic]+(fadeReg<<1),color);
			 
			 if (color && !(i&7))
			    {int x,y;
			     for (x=fadeReg,y=0;x>=fadeWidth;x-=fadeWidth,y++);
			     dust[dustHead].x=x+px;
			     dust[dustHead].y=y+py;
			     dustAge[dustHead]=fadeCount;
			     dustHead=(dustHead+1)&(NMDUSTMOTES-1);
			     if (dustHead==dustTail)
				dustTail=(dustTail+1)&(NMDUSTMOTES-1);
			    }
			}
		    }
		}
	     fadeCount++;
	     /* kill old dust */
	     while (dustAge[dustTail]<fadeCount-25 && dustHead!=dustTail)
		dustTail=(dustTail+1)&(NMDUSTMOTES-1);
	     /* draw dust */
	     if (dustHead!=dustTail)
		{i=dustTail;
		 do
		    {dust[i].y+=1;
		     line[1]=dust[i];
		     line[0]=dust[i];		 
		     EZ_line(COMPO_TRANS|ECD_DISABLE|SPD_DISABLE,
			     greyTable[31-fadeCount+dustAge[i]],line,
			     NULL);
		     
		     i=(i+1)&(NMDUSTMOTES-1);
		    }
		 while (i!=dustHead);
		}
	    }
	}
     
     /* text window */
     if (slidePos[selectedButton]!=-1 && inventoryNum[selectedButton])
	{int nmLines=1;	 
	 char *c;
	 char *string;
	 char buffer[80];
	 int bpos;
	 string=getText(LB_INVTEXT,slidePos[selectedButton]+
			textStart[selectedButton]);
	 for (c=string;*c;c++)
	    if (*c=='\n')
	       nmLines++;
	 p.y=(INVHEIGHT-9-111)-((getFontHeight(1)-1)*nmLines)/2;
	 p.y+=13;
	 bpos=0;
	 for (c=string;;c++)
	    {buffer[bpos++]=*c;
	     if (*c=='\n' || *c==0)
		{buffer[bpos-1]=0;
		 p.x=-(getStringWidth(1,buffer))/2;
		 drawString(p.x,p.y,1,buffer);
		 bpos=0;
		 p.y+=getFontHeight(1)-1;
		}
	     if (!*c)
		break;
	    }
	}

     lastData=data;
     data=lastInputSample;
     changeData=lastData ^ data;

     if (!fade)
	{if ((changeData & PER_DGT_D) && !(data & PER_DGT_D))
	    if (selectedButton<4) 
	       {selectedButton++;
		playStaticSound(ST_INTERFACE,0);
	       }
	 if ((changeData & PER_DGT_U) && !(data & PER_DGT_U))
	    if (selectedButton>0) 
	       {selectedButton--;
		playStaticSound(ST_INTERFACE,0);
	       }
	 if ((changeData & PER_DGT_L) && !(data & PER_DGT_L))
	    if (canGoDown!=-1) 
	       {slidePos[selectedButton]=canGoDown;
		playStaticSound(ST_INTERFACE,1);
	       }
	 if ((changeData & PER_DGT_R) && !(data & PER_DGT_R))
	    if (canGoUp!=-1) 
	       {slidePos[selectedButton]=canGoUp;     
		playStaticSound(ST_INTERFACE,1);
	       }
	 if ((selectedButton==4 || !(data & PER_DGT_S)) &&
	     !(data & (PER_DGT_A|PER_DGT_B|PER_DGT_C)))
	    {extern int quitRequest;
	     quitRequest=1;
	     stopAllSound(54);
	     return;
	    }
#ifndef NOCHEATS	 
	 if ((changeData & PER_DGT_X) && !(data & PER_DGT_X))
	    {currentState.health=currentState.nmBowls*200;
	     for (i=0;i<WP_NMWEAPONS;i++)
		currentState.weaponAmmo[i]=weaponMaxAmmo[i];
	    }
	 if ((changeData & PER_DGT_Y) && !(data & PER_DGT_Y))
	    currentState.inventory^=INV_FEATHER;
#endif
	 if (!(data & PER_DGT_S))
	    quitEnable=1;
	 if (quitEnable && (data & PER_DGT_S))
	    {*mapState=slidePos[0];
	     resetPics();
	     EZ_closeCommand();
	     SCL_DisplayFrame();
	     EZ_openCommand();
	     EZ_closeCommand();
	     SCL_DisplayFrame();	    
	     stopAllSound(54);
	     return;
	    }
	}

     EZ_closeCommand();
     SCL_DisplayFrame();
     if (frameCount++==2)
	SCL_SetColOffset(SCL_OFFSET_A,SCL_SP0|SCL_NBG0,
			 0,0,0);
     

    } 
}

void dlg_setupSlideIn(void)
{int i;
 for (i=0;i<nmItems;i++)
    {dlgItem[i].x2=f(dlgItem[i].xp);
     dlgItem[i].y2=f(dlgItem[i].yp);
     dlgItem[i].x1=dlgItem[i].x2+(i&1?-300:300);
     dlgItem[i].y1=dlgItem[i].y2;
     dlgItem[i].xv=0;
     dlgItem[i].yv=0;     
     if (dlgItem[i].type==IT_RECT)
	{dlgItem[i].x1=0;
	}
    }
}

void dlg_setupSlideOut(void)
{int i;
 for (i=0;i<nmItems;i++)
    {dlgItem[i].x1=f(dlgItem[i].xp);
     dlgItem[i].y1=f(dlgItem[i].yp);
     dlgItem[i].x2=dlgItem[i].x2+(i&1?-300:300);
     dlgItem[i].y2=dlgItem[i].y2;
     dlgItem[i].xv=0;
     dlgItem[i].yv=0;     
     if (dlgItem[i].type==IT_RECT)
	{dlgItem[i].x2=0;
	}
    }
}

void dlg_setupNoSlide(void)
{int i;
 for (i=0;i<nmItems;i++)
    {dlgItem[i].x1=f(dlgItem[i].xp);
     dlgItem[i].y1=f(dlgItem[i].yp);
     dlgItem[i].x2=f(dlgItem[i].xp);
     dlgItem[i].y2=f(dlgItem[i].yp);
     dlgItem[i].xv=0;
     dlgItem[i].yv=0;     
    }
}

void dlg_runSlideIn(void)
{Fixed32 f;
 int i;
 for (f=0;f<F(1);f+=(F(1)/32))
    {for (i=0;i<nmItems;i++)
	{dlgItem[i].xp=evalHermite(f,F(dlgItem[i].x1),
				   F(dlgItem[i].x2),0,dlgItem[i].xv);
	 dlgItem[i].yp=evalHermite(f,F(dlgItem[i].y1),
				   F(dlgItem[i].y2),0,dlgItem[i].yv);
	 if (dlgItem[i].type==IT_RECT)
	    dlgItem[i].w=abs(2*f(dlgItem[i].xp));
	}

     EZ_openCommand();
     EZ_sysClip();
     EZ_localCoord(320/2,240/2);
     dlg_draw(currentButton,0);
     EZ_closeCommand();
     SCL_DisplayFrame();
    }
 
 for (i=0;i<nmItems;i++)
    {dlgItem[i].xp=F(dlgItem[i].x2);
     dlgItem[i].yp=F(dlgItem[i].y2);
    }
}




