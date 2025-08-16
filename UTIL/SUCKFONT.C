#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

unsigned char picdata[512][512];
unsigned short picPal[256];
int picWidth,picHeight;

FILE *ofile;

void readBmp(char *filename)
{FILE *ifile;
 char buff[160];
 int width,height,i;
 ifile=fopen(filename,"rb");
 if (!ifile)
    {printf("Error: can't read file %s.\n",filename);
     exit(-1);
    }
 fread(buff,14+4,1,ifile);
 fread(&width,4,1,ifile);
 fread(&height,4,1,ifile);
 picWidth=width;
 picHeight=height;
 fseek(ifile,14+40,0);
 if (picWidth>512 || picHeight>512)
    {printf("Pic toooo big\n");
     exit(-1);
    }
 /* read pallete */
 for (i=0;i<256;i++)
    {unsigned char r,g,b,foo;
     fread(&b,1,1,ifile);
     fread(&g,1,1,ifile);
     fread(&r,1,1,ifile);
     fread(&foo,1,1,ifile);
     b=b>>3;
     g=g>>3;
     r=r>>3;
     picPal[i]=0x8000|(b<<10)|(g<<5)|r;
    }
 picPal[255]=0x0000;
 for (i=picHeight-1;i>=0;i--)
    {fread(picdata[i],(picWidth+3)&0xfffffffc,1,ifile);
    }
 fclose(ifile);
}

void writeInt(int i)
{printf("%d,",i >> 24);
 printf("%d,",(i >> 16)&0xff);
 printf("%d,",(i >> 8)&0xff);
 printf("%d,",i&0xff);
}

void writeShort(int i)
{printf("%d,",(i >> 8)&0xff);
 printf("%d,",i&0xff);
}

void writeChar(unsigned char i)
{printf("%3d,",i);
}

void mapColors(void)
{int x,y,i;
 int colorList[18];
 int nmColors=0;
 for (i=0;i<18;i++)
    colorList[i]=0;
 colorList[nmColors++]=255; /* 0 is always transparent */
 for (y=0;y<picHeight;y++)
    for (x=0;x<picWidth;x++)
       {for (i=0;i<nmColors;i++)
	   if (colorList[i]==picdata[y][x])
	      break;
	if (i==nmColors)
	   {assert(nmColors<16);
	    colorList[nmColors++]=picdata[y][x];
	   }
	picdata[y][x]=i;
       }
 /* write color list */
 for (i=0;i<16;i++)
    writeShort(picPal[colorList[i]]);
}

int findStrip(int startLine)
{int y;
 for (y=startLine;y<picHeight;y++)
    {if (picdata[y][0]!=0)
	return y;
    }
 return -1;
}

char font[256][64][64];
char widths[256];
int fontHeight;

int suckChars(int baseLine,unsigned char *charMap)
{int xbase,xlast,x,y;
 int nmChars,width;
 int topLine=baseLine-fontHeight+1;
 nmChars=0;
 for (xbase=1;xbase<picWidth;xbase++)
    {for (y=0;y<fontHeight;y++)
	if (picdata[y+topLine][xbase]!=0)
	   break;
     if (y==fontHeight)
	continue;
     /* we found a character */
     for (xlast=xbase;xlast<picWidth;xlast++)
	{for (y=0;y<fontHeight;y++)
	    if (picdata[y+topLine][xlast]!=0)
	       break;
	 if (y==fontHeight)
	    break;
	}
     /* char is bounded by xbase and xlast */
     width=xlast-xbase;
     widths[(int)charMap[nmChars]]=width;
     for (y=0;y<fontHeight;y++)
	for (x=0;x<width;x++)
	   font[(int)charMap[nmChars]][y][x]=picdata[y+topLine][x+xbase];
     xbase=xlast;
     nmChars++;
    }
 return nmChars;
}

void dumpChars(void)
{int i,x,y,odd,accum;
 for (i=0;i<256;i++)
    {writeChar(widths[i]);
     if (!(i % 16))
	printf("\n");
    }
 printf("\n");
 for (i=0;i<256;i++)
    {if (widths[i]==0)
	continue;
     for (y=0;y<fontHeight;y++)
	{odd=0;
	 accum=0;
	 for (x=0;x<((widths[i]+1)&~1);x++)
	    {if (odd)
		{accum|=font[i][y][x];
		 writeChar(accum);
		 accum=0;
		}
	     else
		accum|=font[i][y][x]<<4;
	     odd=!odd;
	    }
	 printf("    /* ");
	 for (x=0;x<widths[i];x++)
	    if (font[i][y][x])
	       printf("*");
	    else
	       printf(" ");
	 printf(" */\n");
	}
     printf("\n");
    }
}

char buffer[1024];

int main(int argc,char **argv)
{int x,y,i;
 char *map;
 if (argc!=6 && argc!=5)
    {printf("Usage: suckfont fontfile.bmp fontHeight spaceWidth arrayname charMap");
     exit(-1);
    }
 if (argc==6)
    map=argv[5];
 else
    {char name[80];
     FILE *f;
     strcpy(name,argv[1]);
     if (!strchr(name,'.'))
	{printf("Barf!\n");
	 exit(0);
	}
     *strchr(name,'.')=0;
     strcat(name,".map");
     f=fopen(name,"rb");
     if (!f)
	{printf("Can't read file %s\n",name);
	 exit(0);
	}
     fread(buffer,1024,1,f);
     fclose(f);
     map=buffer;     
    }
 readBmp(argv[1]);
 sscanf(argv[2],"%d",&fontHeight);
 for (x=0;x<512;x++)
    for (y=0;y<512;y++)
       if (x>=picWidth || y>=picHeight)
	  picdata[y][x]=255;
 printf("unsigned char %s[] = {",argv[4]);
 writeShort(fontHeight);
 mapColors();
 for (i=0;i<256;i++)
    widths[i]=0;
 sscanf(argv[3],"%d",&x);
 widths[32]=x;
 memset(font,0,sizeof(font));

 y=0;
 while ((y=findStrip(y))!=-1)
    {map+=suckChars(y,map);
     y++;
    }
 
 dumpChars();

 printf("};\n");
 return 0;
}
