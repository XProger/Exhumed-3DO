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

#define SAMPLESPERSTEP (44100/30)
void main(int argc,char **argv)
{FILE *ofile;
 int steps;
 char out;
 int i,j,tot;
 if (argc!=3)
    {printf("Barf!\n");
     exit(0);
    }
 loadWave(argv[1]);
 printf("Wave BPS:%d RATE:%d SIZE:%d\n",waveBPS,waveSampleRate,waveSize);
 ofile=fopen(argv[2],"wb");
 steps=(waveSize/4)/SAMPLESPERSTEP; 
        /*steps= # of 30th second chunks in wave file*/
 printf("%d steps.\n",steps);
 for (i=0;i<steps;i++)
    {tot=0;
     for (j=i*SAMPLESPERSTEP;j<(i+1)*SAMPLESPERSTEP;j++)
	tot+=abs(getSampleL(j))+abs(getSampleR(j));
     printf("%d ",tot>>18);
     out=tot>>18;
     fwrite(&out,1,1,ofile);
    }
 fclose(ofile);
}


