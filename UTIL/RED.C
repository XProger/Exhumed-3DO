#include <stdlib.h>
#include <stdio.h>

char waveBuff[1024*1024];
int waveSize,waveSampleRate,waveBPS;

int main(int argc,char **argv)
{FILE *ifile;
 FILE *ofile;
 int size,min,max;
 short sample;
 ifile=fopen(argv[1],"rb");
 if (!ifile)
    {printf("Can't load wave file %s\n",argv[1]);
     exit(0);
    }
 {char name[160];
  char *c;
  c=argv[1]+strlen(argv[1])-1;
  while (c>argv[1] && *(c-1)!='\\') c--;
  strcpy(name,c);
  for (c=name;*c && *c!='.';c++) ;
  *c=0;
  strcat(name,".red");
  ofile=fopen(name,"wb");
  if (!ofile)
     {printf("Can't open output file (%s)\n",name);
      exit(0);
     }
 }
 
 fread(waveBuff,8,1,ifile);
 fread(waveBuff,4,1,ifile);
 waveBuff[4]=0;
 if (strcmp(waveBuff,"WAVE"))
    {printf("Wave file '%s' bad! %s\n",argv[1],waveBuff);
     exit(-1);
    }
 fread(waveBuff,4,1,ifile);
 fread(&size,4,1,ifile);

 fread(waveBuff,4,1,ifile);
 fread(&waveSampleRate,4,1,ifile);
 if (waveSampleRate!=44100)
    {printf("Wave file not 44.1K Hz!\n");
     exit(0);
    }
 fread(waveBuff,6,1,ifile);
 waveBPS=0;
 fread(&waveBPS,2,1,ifile);
 fread(waveBuff,size-16,1,ifile);
 fread(waveBuff,4,1,ifile);
 waveBuff[4]=0;
 if (strcmp(waveBuff,"data"))
    {printf("Wave file '%s' half bad! %s\n",argv[1],waveBuff);
     exit(-1);
    }
 fread(&waveSize,4,1,ifile);
 
 min=0;
 max=0;
 while (waveSize>0)
    {if (waveSize>1024*1024)
	{fread(waveBuff,1024*1024,1,ifile);
	 fwrite(waveBuff,1024*1024,1,ofile);
	 waveSize-=1024*1024;
	 printf("Meg!\n");
	}
     else
	{fread(waveBuff,waveSize,1,ifile);
	 fwrite(waveBuff,waveSize,1,ofile);
	 waveSize-=waveSize;
	}
    }
 printf("Wavefile rate:%d size:%d BPS:%d\n",waveSampleRate,waveSize,waveBPS);
 fclose(ifile);
 fclose(ofile);
 return 0;
}

