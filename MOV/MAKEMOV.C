#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int picWidth,picHeight;
unsigned char picdata[512][512][3];
unsigned char lastFrame[512][512][3];

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
 if (picWidth>320 || picHeight>240)
    {printf("Pic too big\n");
     exit(-1);
    }
 fseek(ifile,14+40,0);

 for (i=239-(240-picHeight)/2;i>=0;i--)
    {fread(picdata[i]+((320-picWidth)/2),
	   picWidth*3,1,ifile);
     fread(buff,1,(4-(picWidth&3))&3,ifile);
    }
 fclose(ifile);
}

int waveSize,waveSampleRate,waveBPS;
int waveChannels;
int waveLoopStart; /* -1 if no loop */
int waveSpeed;
int waveChannels;

FILE *waveFile;

void loadWaveHeader(char *name)
{int size;
 char waveBuff[1024];
 waveFile=fopen(name,"rb");
 if (!waveFile)
    {printf("Can't load wave file %s\n",name);
     exit(0);
    }
 waveLoopStart=-1;
 fread(waveBuff,8,1,waveFile);
 fread(waveBuff,4,1,waveFile);
 waveBuff[4]=0;
 if (strcmp(waveBuff,"WAVE"))
    {printf("Wave file '%s' bad! %s\n",name,waveBuff);
     exit(-1);
    }
 fread(waveBuff,4,1,waveFile);
 fread(&size,4,1,waveFile);

 fread(waveBuff,2,1,waveFile);
 waveChannels=0;
 fread(&waveChannels,2,1,waveFile);
 printf("wave has %d channels\n",waveChannels);
 fread(&waveSampleRate,4,1,waveFile);
 fread(waveBuff,6,1,waveFile);
 waveBPS=0;
 fread(&waveBPS,2,1,waveFile);
 fread(waveBuff,size-16,1,waveFile);

 do /* read blocks until we find the data one */
    {fread(waveBuff,4,1,waveFile);
     waveBuff[4]=0;
     if (strcmp(waveBuff,"data"))
	{fread(&size,4,1,waveFile);
	 printf("Got unknown block %s, size %d\n",waveBuff,size);
	 fread(waveBuff,size,1,waveFile);
	 if (feof(waveFile))
	     {printf("Wave file '%s' half bad! %s\n",name,waveBuff);
	      exit(-1);
	     }
	}
    }
 while (strcmp(waveBuff,"data")) ;

 fread(&waveSize,4,1,waveFile);
 if (!waveSize)
    {printf("Wave size==0!\n");
     exit(-1);
    }

 {int octave,fns;
  double x;
  x=log(waveSampleRate)/log(2.0);
  x=x-log(44100.0)/log(2.0);
  octave=floor(x);
  fns=fabs((x-floor(x))*1024.0);
  assert((fns & 0x03ff)==fns);
  waveSpeed=(((octave<<11)&0x7800)|fns);
 }

}


FILE *ofile;

void writeInt(int i)
{fputc(i >> 24,ofile);
 fputc((i >> 16)&0xff,ofile);
 fputc((i >> 8)&0xff,ofile);
 fputc(i & 0xff,ofile);
}

void writeShort(short i)
{fputc(i >> 8,ofile);
 fputc(i & 0xff,ofile);
}

void writeChar(char i)
{fputc(i,ofile);
}

char waveBuff[500000];
void writeSound(int amount)
{int i,read,readAmount;
 int wrote=0;
 int saveAmount=amount;
 if (!waveFile)
    {writeInt(0);
     return;
    }
 assert(waveBPS==16);
 assert(waveChannels==2);
 writeInt(amount);
 if (amount*4>waveSize)
    readAmount=waveSize/4;
 else
    readAmount=amount;
 waveSize-=readAmount*4;
 if (readAmount>0)
    {read=fread(waveBuff,4,readAmount,waveFile);
     assert(read==readAmount);
     for (i=0;i<read*4;i+=4)
	{writeChar(waveBuff[i+1]);
	 writeChar(waveBuff[i]);
	 wrote+=2;
	}
     for (i=2;i<read*4;i+=4)
	{writeChar(waveBuff[i+1]);
	 writeChar(waveBuff[i]);
	 wrote+=2;
	}
    }
 amount-=readAmount;
 for (;amount;amount--)
    {writeInt(0);
     wrote+=4;
    }
 assert(wrote=saveAmount*4);
}

#define MAXRUN 65534

#define RGB(r,g,b) (0x8000|(((r)>>3)<<10)|(((g)>>3)<<5)|(((b)>>3)<<0))

int frNm;
int lose;
int same(int x,int y)
{int diff;
 diff=0;
 diff+=abs((picdata[y][x][0]>>3)-(lastFrame[y][x][0]>>3));
 diff+=abs((picdata[y][x][1]>>3)-(lastFrame[y][x][1]>>3));
 diff+=abs((picdata[y][x][2]>>3)-(lastFrame[y][x][2]>>3));
 if (diff<lose)
    return 1;
 return 0;
}

int addFrame(char *bmpName,double fps)
{static char *lastFrameName=NULL;
 static int frameNumber=0;
 int x,y,size,runCount,sx,sy,delay,pixelCount;
 printf("Processing frame %s ",bmpName);
 runCount=0; pixelCount=0;
/* if (!lastFrameName || strcmp(lastFrameName,bmpName))*/
    readBmp(bmpName);
 delay=0;
 x=0;
 y=0;
 lastFrameName=bmpName;
 while (1)
    {/* output block of same */
     for (size=0;
	  y<240 && same(x,y) && same(x+1,y);
	  size+=2)
	{picdata[y][x][0]=lastFrame[y][x][0];
	 picdata[y][x][1]=lastFrame[y][x][1];
	 picdata[y][x][2]=lastFrame[y][x][2];
	 picdata[y][x+1][0]=lastFrame[y][x+1][0];
	 picdata[y][x+1][1]=lastFrame[y][x+1][1];
	 picdata[y][x+1][2]=lastFrame[y][x+1][2];
	 x+=2;
	 if (x>=320)
	    {x=0;
	     y++;
	    }
	}
     writeInt(size); runCount++;
     assert(size>=0);
     assert(size<80000);
     if (y>=240)
	break;
     /* output block of differences */
     sx=x;
     sy=y;
     delay=0;
     for (size=0;
	  y<240;
	  size++)
	{if (same(x,y) && same(x+1,y))
	    {if (delay>=4)
		break;
	     delay++;
	    }
	 else
	    delay=0;
	 x+=2;
	 if (x>=320)
	    {x=0;
	     y++;
	    }
	}
     size-=delay;
     writeInt(size*2); runCount++;
     assert(2*size>=0);
     assert(2*size<80000);
     x=sx;
     y=sy;
     for (;size;size--)
	{writeShort(RGB(picdata[y][x][0],
			picdata[y][x][1],
			picdata[y][x][2]));
	 writeShort(RGB(picdata[y][x+1][0],
			picdata[y][x+1][1],
			picdata[y][x+1][2]));
	 pixelCount+=2;
	 x+=2;
	 if (x>=320)
	    {x=0;
	     y++;
	    }
	}
     if (y>=240)
	break;
    }
 writeInt(-1); /* write frame termination character */
 /* write sound for this block */
 writeSound(((double)waveSampleRate)/fps);
 memcpy(lastFrame,picdata,sizeof(picdata));
 printf(" %d runs  %d pixels\n",runCount,pixelCount);
 frameNumber++;
 return runCount;
}


int main(int argc,char **argv)
{char buff[160];
 int frame,i;
 int totalRuns,min,max;
 double fps=20;
 char *soundFile;
 int nmFrames;
 if (argc<4)
    {printf("ARgs!\n");
     exit(-1);
    }
 ofile=fopen(argv[1],"wb");
 if (!ofile)
    {printf("Can't open output file for writing\n");
     exit(-1);
    }

 nmFrames=0;
 for (i=2;i<argc;i++)
    {if (argv[i][0]=='#')
	{sscanf(argv[i],"#%d-%d",&min,&max);
	 nmFrames+=max-min+1;
	 i++;
	 continue;
	}
     if (argv[i][0]=='-')
	{sscanf(argv[i],"-%lf",&fps);
	 printf("fps=%f\n",fps);
	 continue;
	}
     if (strstr(argv[i],".bmp"))
	nmFrames++;
    }

 printf("nmFrames=%d\n",nmFrames);

 /* look for lng file */
 {FILE *ifile;
  char *foo;
  foo=NULL;
  for (i=2;i<argc;i++)
     if (strstr(argv[i],".lng"))
	foo=argv[i];
  if (foo)
     {int len;
      printf("Reading language file from %s\n",foo);
      ifile=fopen(foo,"rb");
      len=fread((char *)picdata,1,100000,ifile);
      len=1024*6;
      fwrite((char *)picdata,1,len,ofile);
      fclose(ifile);
     }
  else
     {writeInt(0);
     }
 }

 /* look for sound file */
 soundFile=NULL;
 for (i=2;i<argc;i++)
    if (strstr(argv[i],".wav"))
       soundFile=argv[i];
 if (soundFile)
    loadWaveHeader(soundFile);
 else
    waveFile=NULL;

 /* write file header */
 fprintf(ofile,"MOVIE!");
 writeShort(nmFrames); /* nmframes */
 if (soundFile)
    {writeShort(waveBPS);
     writeShort(waveChannels);
     writeShort(waveSpeed);
     writeShort(0);
    }
 else
    {writeShort(0);
     writeShort(0);
     writeShort(0);
     writeShort(0);
    }
 writeSound(32768);
 printf("waveSampleRate=%d\n",waveSampleRate);

 memset(lastFrame,0,sizeof(lastFrame));
 frNm=0;
 totalRuns=0;
 lose=1;
 for (frame=2;frame<argc;frame++)
    {if (argv[frame][0]=='#')
	{sscanf(argv[frame],"#%d-%d",&min,&max);
	 frame++;
	 for (i=min;i<=max;i++)
	    {frNm++;
	     if (frNm%3)
		lose=5;
	     else
		lose=1;
	     lose=1;
	     sprintf(buff,argv[frame],i);
	     totalRuns+=addFrame(buff,fps);
	    }
	 continue;
	}
     if (strstr(argv[frame],".bmp"))
	{frNm++;
	 if (frNm%3)
	    lose=5;
	 else
	    lose=1;
	 lose=1;
	 totalRuns+=addFrame(argv[frame],fps);
	}
    }
 printf("Total runs %d\n",totalRuns);
 fclose(ofile);
 exit(0);
}
