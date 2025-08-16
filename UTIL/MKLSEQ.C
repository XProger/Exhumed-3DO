#include <stdlib.h>
#include <stdio.h>

char waveBuff[1024*1024*8];
int waveSize,waveSampleRate,waveBPS;
int waveLoopStart; /* -1 if no loop */

void loadWave(char *name)
{FILE *ifile;
 int size;
 ifile=fopen(name,"rb");
 if (!ifile)
    {printf("Can't load wave file %s\n",name);
     exit(0);
    }
 waveLoopStart=-1;
 fread(waveBuff,8,1,ifile);
 fread(waveBuff,4,1,ifile);
 waveBuff[4]=0;
 if (strcmp(waveBuff,"WAVE"))
    {printf("Wave file '%s' bad! %s\n",name,waveBuff);
     exit(-1);
    }
 fread(waveBuff,4,1,ifile);
 fread(&size,4,1,ifile);

 fread(waveBuff,4,1,ifile);
 fread(&waveSampleRate,4,1,ifile);
 fread(waveBuff,6,1,ifile);
 waveBPS=0;
 fread(&waveBPS,2,1,ifile);
 fread(waveBuff,size-16,1,ifile);
 fread(waveBuff,4,1,ifile);
 waveBuff[4]=0;
 if (strcmp(waveBuff,"data"))
    {printf("Wave file '%s' half bad! %s\n",name,waveBuff);
     exit(-1);
    }
 fread(&waveSize,4,1,ifile);
 if (waveSize>1024*1024*8)
    {printf("Wave is too large\n");
     exit(-1);
    }
 fread(waveBuff,1,waveSize,ifile);
 fclose(ifile);
}

short getSampleL(int sNm)
{return (waveBuff[sNm*4+1]<<8)|waveBuff[sNm*4];
}

short getSampleR(int sNm)
{return (waveBuff[sNm*4+3]<<8)|waveBuff[sNm*4+2];
}

#define MAX 20000

int nmFrames;
int nmChunks;
short frameStart[MAX];
short chunkx[MAX],chunky[MAX],chunkpic[MAX];

FILE *ofile;

void writeShort(int s)
{fwrite(&s,2,1,ofile);
}

#define XOFFS -48
#define YOFFS -106

#define SAMPLESPERSTEP (44100/30)
void main(int argc,char **argv)
{int steps;
 char out;
 int i,j,tot,mouth,brow,blink;
 if (argc!=9)
    {printf("Usage: mklseq.exe waveFile outFile normalFace blinkFace browFace blinkBrowFace mouth1 mouth2\n");
     exit(0);
    }
 loadWave(argv[1]);
 printf("Wave BPS:%d RATE:%d SIZE:%d\n",waveBPS,waveSampleRate,waveSize);
 ofile=fopen(argv[2],"wb");
 steps=(waveSize/4)/SAMPLESPERSTEP; 
        /*steps= # of 30th second chunks in wave file*/
 printf("%d steps.\n",steps);
 nmFrames=0;
 nmChunks=0;
 mouth=0;
 brow=-30;
 for (i=0;i<12;i++)
    {tot=0;
     for (j=i*SAMPLESPERSTEP;j<(i+1)*SAMPLESPERSTEP;j++)
	tot+=abs(getSampleL(j))+abs(getSampleR(j));
     printf("%d ",tot>>18);
     out=tot>>18;

     switch (mouth)
	{case 0:
	    if (out>5) mouth=1;
	    break;
	 case 1:
	    if (out<5) mouth=0;
	    if (out>10) mouth=2;
	    break;
	 case 2:
	    if (out<10) mouth=1;
	    break;
	   }
     brow+=mouth;
     if (brow>50) brow=-50;
     if ((i%45)==0)
	blink=1;
     else
	blink=0;

     frameStart[nmFrames++]=nmChunks;
     chunkx[nmChunks]=XOFFS;
     chunky[nmChunks]=YOFFS;
     blink=i&1;
     brow=i&2;
     mouth=i>>2;
     switch (((!!(brow>0))<<1)|(!!blink))
	{case 0: chunkpic[nmChunks]=0; break;
	 case 1: chunkpic[nmChunks]=1; break;
	 case 2: chunkpic[nmChunks]=2; break;
	 case 3: chunkpic[nmChunks]=3; break;
	}
     nmChunks++;
     switch (mouth)
	{case 1:
	    {chunkx[nmChunks]=XOFFS+36;
	     chunky[nmChunks]=YOFFS+63;
	     chunkpic[nmChunks]=4;
	     nmChunks++;
	     break;
	    }
	 case 2:
	    {chunkx[nmChunks]=XOFFS+36;
	     chunky[nmChunks]=YOFFS+63;
	     chunkpic[nmChunks]=5;
	     nmChunks++;
	     break;
	    }
	   }
    }
 {int i;
  short s;
  fwrite("PS",2,1,ofile);
  writeShort(0); /* nm sounds */
  writeShort(6); /* nm tiles */
  for (i=3;i<9;i++)
     {fwrite(argv[i],strlen(argv[i]),1,ofile);
      fwrite("\n",1,1,ofile);
     }
  writeShort(1); /* nm sequences */
  writeShort(0);
  writeShort(nmFrames);
  for (i=0;i<nmFrames;i++)
     writeShort(frameStart[i]);
  for (i=0;i<nmFrames;i++)
     writeShort(0);
  writeShort(nmChunks);
  for (i=0;i<nmChunks;i++)
     writeShort(chunkx[i]);
  for (i=0;i<nmChunks;i++)
     writeShort(chunky[i]);
  for (i=0;i<nmChunks;i++)
     writeShort(chunkpic[i]);
  for (i=0;i<nmChunks;i++)
     writeShort(0);

  /* sounds */
  for (i=0;i<nmFrames;i++)
     writeShort(0);
 }
 fclose(ofile);
}


