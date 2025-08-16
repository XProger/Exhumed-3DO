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
 if (waveSize>=1024*1024*8)
    {printf("Wave is too large\n");
     exit(-1);
    }
 fread(waveBuff,1,waveSize,ifile);
 fclose(ifile);
}

#define MAX 20000

int nmFrames;
char lipPos[MAX];

FILE *ofile;

void writeShort(int i)
{fputc(i >> 8,ofile);
 fputc(i & 0xff,ofile);
}

void writeChar(char i)
{fputc(i,ofile);
}

#define XOFFS -48
#define YOFFS -106

int SAMPLESPERSTEP;
int pal=0;
int stereo=0;

short getSample(int sNm)
{if (!stereo)
    {return (waveBuff[sNm*2+1]<<8)|((unsigned char)waveBuff[sNm*2]);
    } 
 return 
    (((waveBuff[sNm*4+1]<<8)|((unsigned char)waveBuff[sNm*4]))+
     ((waveBuff[sNm*4+3]<<8)|((unsigned char)waveBuff[sNm*4+2])))/2;
}

void main(int argc,char **argv)
{int steps;
 int THRESH1,THRESH2,THRESH3,THRESH4,delta,lastMouth,dmouth;
 int i,j,tot,mouth,brow,blink,zero,lip,loudestSample,lastVol,peakVol;
 if (argc<3)
    {printf("Usage: mklseq.exe voiceFile outFile\n");
     exit(0);
    }
 SAMPLESPERSTEP=22050/30;
 for (i=4;i<argc;i++)
    {if (!stricmp(argv[i],"-pal"))
	{pal=1;
	 SAMPLESPERSTEP=22050/25;
	 continue;
	}
     if (!stricmp(argv[i],"-stereo"))
	{stereo=1;
	 continue;
	}
     printf("unknown arg!\n");
     exit(-1);
    }
 loadWave(argv[1]);
 printf("Wave BPS:%d RATE:%d SIZE:%d\n",waveBPS,waveSampleRate,waveSize);
 ofile=fopen(argv[2],"wb");
 if (stereo)
    steps=(waveSize/4)/SAMPLESPERSTEP; 
 else
    steps=(waveSize/2)/SAMPLESPERSTEP; 
 /*steps= # of 30th second chunks in wave file*/
 printf("%d steps.\n",steps);
 nmFrames=0;
 mouth=0;
 brow=-30;
 /* find loudest sample */
 loudestSample=0;
 for (i=0;i<steps;i++)
    {tot=0;
     for (j=i*SAMPLESPERSTEP;j<(i+1)*SAMPLESPERSTEP;j++)
	{int s=getSample(j);
	 tot+=abs(s);
	}
     if (tot>loudestSample)
	loudestSample=tot;     
    }
 THRESH1=((double)loudestSample)*0.2;
 THRESH2=((double)loudestSample)*0.25;
 THRESH3=((double)loudestSample)*0.35;
 THRESH4=((double)loudestSample)*0.40;
 delta=((double)loudestSample)*0.05;
 printf("Loudest sample=%d (%d/%d/%d/%d)\n",
	loudestSample,THRESH1,THRESH2,THRESH3,THRESH4);
 lastVol=0;
 peakVol=0;
 mouth=0;
 lastMouth=0;
 for (i=0;i<steps;i++)
    {tot=0;
     for (j=i*SAMPLESPERSTEP;j<(i+1)*SAMPLESPERSTEP;j++)
	{int s=getSample(j);
	 tot+=abs(s);
	}
     
     if (tot>lastVol+delta)
	mouth=1;
     if (mouth==1)
	{if (tot>peakVol)
	    peakVol=tot;
	 if (tot>peakVol*0.95)
	    mouth=2;
	}
     if (tot<peakVol/2)
	{mouth=0;
	 peakVol=0;
	}
     lastVol=tot;
     
     if (tot<THRESH2)
	if (mouth==2)
	   mouth=1;

     dmouth=mouth;
     if (dmouth==0 && lastMouth==2)
	dmouth=1;
     if (dmouth==2 && lastMouth==0)
	dmouth=1;
     lastMouth=dmouth;
#if 0
     switch (mouth)
	{case 0:
	    if (tot>THRESH2) mouth=1;
	    break;
	 case 1:
	    if (tot<THRESH1) mouth=0;
	    if (tot>THRESH4) mouth=2;
	    break;
	 case 2:
	    if (tot<THRESH3) mouth=1;
	    break;
	   }
#endif
     brow+=dmouth;
     if (brow>50) brow=-50;
     if ((i%45)==0 || (i%45)==1)
	blink=1;
     else
	blink=0;

     {short c;
      int q;
      c=0;
      c+=dmouth;
      if (brow)
	 c+=6;
      if (blink)
	 c+=3;
      /* c=(((!!(brow>0))<<1)|(!!blink));
	 c+=4*dmouth; */
      lipPos[nmFrames++]=c;
      printf("%d ",dmouth);
      for (q=0;q<((double)tot)*(75.0/loudestSample);q++)
	 printf("#");
      printf("\n");
     }
    }

 writeShort(steps);
 for (i=0;i<nmFrames;i++)
    writeChar(lipPos[i]);
 fclose(ofile);
}


