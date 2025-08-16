#include <stdlib.h>
#include <stdio.h>

int forceTransp;
int force64x64;
int rle;

unsigned char picdata[512][512];
unsigned short picPal[256];
unsigned short lastPallete[256];
int picWidth,picHeight;

unsigned char rleBuffer[1024*50];
unsigned char tempBuffer[1024*50];
int tempBufferSize;
FILE *ofile;

int rlePic(FILE *ofile)
{int i,pos,size;
 int nmWritten=0;
 int x1,y1;
 pos=0;
 for (y1=0;y1<picHeight;y1++)
    for (x1=0;x1<picWidth;x1++)
       {tempBuffer[pos++]=picdata[y1][x1];
       }
 tempBufferSize=pos;
 pos=0;
 while (pos<tempBufferSize)
    {/* output block of blank space */
     for (size=0;size<255 && pos+size<tempBufferSize && 
	  tempBuffer[pos+size]==0;size++) ;
     if (ofile)
	fputc(size,ofile); 
     nmWritten++;
     pos+=size;
     /* output block of nonblank space */
     for (size=0;size<255 && pos+size<tempBufferSize && 
	  tempBuffer[pos+size]!=0;size++) ;
     if (ofile)
	fputc(size,ofile); 
     nmWritten++;
     if (ofile)
	for (i=0;i<size;i++)
	   fputc(tempBuffer[i+pos],ofile);
     nmWritten+=size;
     pos+=size;
    }
 while (nmWritten&0x3)
    {if (ofile)
	fputc(0,ofile);
     nmWritten++;
    }
 return nmWritten;
}

int rle2Pic(FILE *ofile)
{int pos,size;
 int nmWritten=0;
 int x1,y1;
 pos=0;


 picWidth=24;
 picHeight=18;


 for (y1=0;y1<picHeight;y1++)
    for (x1=0;x1<picWidth;x1++)
       {if (x1>=18)
	   tempBuffer[pos++]=255;
	else
	   tempBuffer[pos++]=picdata[y1][x1];
       }
 tempBufferSize=pos;
 pos=0;
 while (pos<tempBufferSize)
    {/* output block of blank space */
     for (size=0;size<255 && pos+size<tempBufferSize && 
	  tempBuffer[pos+size]!=255;size++) ;
     if (ofile)
	fputc(size,ofile); 
     nmWritten++;
     pos+=size;
     /* output block of nonblank space */
     for (size=0;size<255 && pos+size<tempBufferSize && 
	  tempBuffer[pos+size]==255;size++) ;
     if (ofile)
	fputc(size,ofile); 
     nmWritten++;
     pos+=size;
    }
 while (nmWritten&0x3)
    {if (ofile)
	fputc(0,ofile);
     nmWritten++;
    }
 return nmWritten;
}


void readBmp(char *filename)
{FILE *ifile;
 char buff[160];
 int width,height,i,j;
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

/* picPal[255]=picPal[0]; */
/* picPal[255]=0;*/ /* this is the last change I made */
#if 1
 {unsigned short swap;
  swap=picPal[0];
  picPal[0]=picPal[255];
  picPal[255]=swap;
 }
 if (forceTransp)
    picPal[0]=0; 
#endif

 for (i=picHeight-1;i>=0;i--)
    {fread(picdata[i],(picWidth+3)&0xfffffffc,1,ifile);
     for (j=picWidth;j<=((picWidth+7)&(~7));j++)
	picdata[i][j]=255;
    }

#if 1
 for (i=0;i<picHeight;i++)
    for (j=0;j<=((picWidth+7)&(~7));j++)
       {if (picdata[i][j]==0)
	   {picdata[i][j]=255;
	   }
       else
	  if (picdata[i][j]==255)
	     {picdata[i][j]=0;
	     }
       }
#endif

 fclose(ifile);
}

void writeShort(short i)
{fputc(i >> 8,ofile);
 fputc(i & 0xff,ofile);
}

void writeInt(int i)
{fputc(i >> 24,ofile);
 fputc((i >> 16)&0xff,ofile);
 fputc((i >> 8)&0xff,ofile);
 fputc(i & 0xff,ofile);
}

void writeChar(char i)
{fputc(i,ofile);
}

int main(int argc,char **argv)
{int i,x,y,j;
 int size;
 char *c;
 char buffer[320];
 FILE *ifile;
 if (argc<3)
    {printf("Arg!\n");
     exit(-1);
    }

 ofile=NULL;
 ifile=NULL;
 forceTransp=0;
 force64x64=0;
 for (i=1;i<argc;i++)
    {if (argv[i][0]=='-')
	{if (!stricmp(argv[i]+1,"transp"))
	    {forceTransp=1;
	     continue;
	    }
	 if (!stricmp(argv[i]+1,"64x64"))
	    {force64x64=1;
	     continue;
	    }
	 if (!stricmp(argv[i]+1,"rle"))
	    {rle=1;
	     continue;
	    }
	 if (!stricmp(argv[i]+1,"rle2"))
	    {rle=2;
	     continue;
	    }
	 printf("Arg! Unknown arg!\n");
	 exit(-1);	     
	}
     if (!ofile)
	ofile=fopen(argv[i],"wb");
     else
	{if (ifile)
	    {printf("Arg!\n");
	     exit(-1);
	    }
	 ifile=fopen(argv[i],"r");
	}
    }
 if (!ofile || !ifile)
    {printf("Arg!\n");
     exit(-1);
    }

 for (i=0;i<256;i++)
    lastPallete[i]=0;
 size=0;
 while ((c=fgets(buffer,100,ifile)))
    {if (strchr(c,'\n'))
	*strchr(c,'\n')=0;
     printf("Reading %s\n",c);
     readBmp(c);
     picWidth=(picWidth+7)&(~7);
     if (force64x64)
	{picHeight=64;
	 picWidth=64;
	}
     size+=12;
     if (rle)
	{if (rle==1)
	    size+=rlePic(NULL);
	 else
	    size+=rle2Pic(NULL);
	}
     else
	size+=picWidth*picHeight;
     for (j=0;j<256;j++)
	if (lastPallete[j]!=picPal[j])
	   break;
     if (j==256)
	{size+=4;
	 continue;
	}
     size+=256*2+4;
     for (j=0;j<256;j++)
	lastPallete[j]=picPal[j];
    }
 writeInt(size);
 for (i=0;i<256;i++)
    lastPallete[i]=0;
 printf("size=%d\n",size);
 fclose(ifile);
 ifile=fopen(argv[2],"r");
 while ((c=fgets(buffer,100,ifile)))
    {if (strchr(c,'\n'))
	*strchr(c,'\n')=0;
     if (force64x64)
	memset(picdata,0,sizeof picdata);
     readBmp(c);
     picWidth=(picWidth+7)&(~7);
     if (force64x64)
	{picHeight=64;
	 picWidth=64;
	}
     if (rle)
	{if (rle==1)
	    {writeInt(rlePic(NULL));
	     writeInt(picWidth);
	     writeInt(picHeight);
	     rlePic(ofile);
	    }
	 else
	    {writeInt(rle2Pic(NULL));
	     writeInt(picWidth);
	     writeInt(picHeight);
	     rle2Pic(ofile);
	    }
	}
     else
	{writeInt(picHeight*picWidth);
	 writeInt(picWidth);
	 writeInt(picHeight);
	 for (y=0;y<picHeight;y++)
	    for (x=0;x<picWidth;x++)
	       {writeChar(picdata[y][x]);
	       }
	}

     for (j=0;j<256;j++)
	if (lastPallete[j]!=picPal[j])
	   break;
     if (j==256)
	{writeInt(0);
	 printf("save pal\n");
	 continue;
	}
     writeInt(1);
     for (j=0;j<256;j++)
	{writeShort(picPal[j]);
	 lastPallete[j]=picPal[j];
	}
    }
 fclose(ofile);
 return 0;
}
