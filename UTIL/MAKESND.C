#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

char waveBuff[500000];
int waveSize,waveSampleRate,waveBPS;
int waveLoopStart; /* -1 if no loop */

FILE *ofile;

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

int writeSound(void)
{int i,octave,fns;
 double x;
 int wrote;
 x=log(waveSampleRate)/log(2.0);
 x=x-log(44100.0)/log(2.0);
 octave=floor(x);
 fns=fabs((x-floor(x))*1024.0);
/* printf("Octave=%d fns=%d\n",octave,fns);*/
 assert((fns & 0x03ff)==fns);

 writeInt(waveSize);
 writeInt(((octave<<11)&0x7800)|fns);
 writeInt(waveBPS);
 writeInt(waveLoopStart);

 if (waveBPS==16)
    {/* size better be even */
     assert(!(waveSize & 1));
     wrote=0;
     for (i=0;i<waveSize;i+=2)
	{writeChar(waveBuff[i+1]);
	 wrote++;
	 writeChar(waveBuff[i]);
	 wrote++;
	}
     assert(wrote==waveSize);
    }
 else
    for (i=0;i<waveSize;i++)
       writeChar(waveBuff[i] ^ 0x80);
 return waveSize+12;
}

void loadVoc(char *name)
{int i;
 FILE *ifile;
 int size;
 char type;
 ifile=fopen(name,"rb");
 if (!ifile)
    {printf("Can't load voc file %s\n",name);
     exit(0);
    }
 fread(waveBuff,19,1,ifile);
 waveBuff[19]=0;
 if (strcmp(waveBuff,"Creative Voice File"))
    {printf("Voc file '%s' bad! %s\n",name,waveBuff);
     exit(-1);
    }
 fread(waveBuff,7,1,ifile);
 waveSize=0;
 waveLoopStart=-1;
 /* read blocks */
 while (1)
    {fread(&type,1,1,ifile);
     switch (type)
	{case 0:
	    printf("Terminate\n");
	    return;
	 case 1:
	    size=0;
	    fread(&size,3,1,ifile);
	    printf("Sound Data size=%d\n",size);
	    assert(0);
	    break;
	 case 2:
	    printf("Sound continue\n");
	    assert(0);
	    break;
	 case 3:
	    printf("Silence\n");
	    assert(0);
	    break;
	 case 4:
	    printf("Marker\n");
	    assert(0);
	    break;
	 case 5:
	    printf("Ascii string\n");
	    assert(0);
	    break;
	 case 6:
	    i=0;
	    fread(&i,3,1,ifile);
	    assert(i==2);
	    i=0;
	    fread(&i,2,1,ifile);
	    printf("Repeat %d\n",i);
	    assert(i==0x0ffff);
	    waveLoopStart=waveSize;
	    break;
	 case 7:
	    printf("End Repeat\n");
	    assert(0);
	    break;
	 case 9:
	    {char c;
	     int i;
	     printf("Sound Data\n");
	     size=0;
	     fread(&size,3,1,ifile);
	     printf("Size=%d\n",size);
	     fread(&waveSampleRate,4,1,ifile);
	     printf("SampleRate=%d\n",waveSampleRate);
	     waveBPS=0;
	     fread(&waveBPS,1,1,ifile);
	     printf("Wave BPS=%d\n",waveBPS);
	     assert(waveBPS==8);
	     fread(&c,1,1,ifile);
	     printf("Channels=%d\n",c);
	     assert(c==1);
	     i=0;
	     fread(&i,2,1,ifile);
	     printf("Format=%d\n",i);
	     assert(!i);
	     fread(&i,4,1,ifile);
	     /* read wave data */
	     fread(waveBuff+waveSize,size-12,1,ifile);
	     waveSize+=size-12;
	     break;
	    }
	 default:
	    printf("Unknown block type %d in voc\n",type);
	    assert(0);
	    break;
	   }
    }
 fclose(ifile);
}


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

 do /* read blocks until we find the data one */
    {fread(waveBuff,4,1,ifile);
     waveBuff[4]=0;
     if (strcmp(waveBuff,"data"))
	{fread(&size,4,1,ifile);
	 printf("Got unknown block %s, size %d\n",waveBuff,size);
	 fread(waveBuff,size,1,ifile);
	 if (feof(ifile))
	     {printf("Wave file '%s' half bad! %s\n",name,waveBuff);
	      exit(-1);
	     }
	}
    }
 while (strcmp(waveBuff,"data")) ;

 fread(&waveSize,4,1,ifile);
 if (!waveSize)
    {printf("Wave size==0!\n");
     exit(-1);
    }
 fread(waveBuff,1,waveSize,ifile);
 fclose(ifile);
}


int main(int argc,char **argv)
{assert(argc==3);
 loadWave(argv[1]);
 ofile=fopen(argv[2],"wb");
 writeSound();
 return 0;
}
