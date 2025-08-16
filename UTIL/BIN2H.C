#include <stdlib.h>
#include <stdio.h>

int size;
unsigned char data[1024*1024];
FILE *ofile;

void readBin(char *filename)
{FILE *ifile;
 ifile=fopen(filename,"rb");
 if (!ifile)
    {printf("Error: can't read file %s.\n",filename);
     exit(-1);
    }
 size=fread(data,1,1024*1024,ifile);
 fclose(ifile);
}


void writeChar(unsigned char i)
{fprintf(ofile,"%d,",i);
}

int main(int argc,char **argv)
{int x,y,count=0;
 readBin(argv[1]);
 ofile=fopen(argv[2],"a");
 if (!ofile)
    {printf("Barf!\n");
     exit(-1);
    }
 fprintf(ofile,"unsigned char %s[] = {",argv[3]);
 
 for (y=0;y<size;y++)
    {if (count++>30)
	{count=0;
	 fprintf(ofile,"\n");
	}
     writeChar(data[y]);
    }
 fprintf(ofile,"};\n");
 fclose(ofile);
 return 0;
}
