#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char buff[1024*1024*8];


int main(int argc,char **argv)
{FILE *ifile;
 char name[64];
 int nmFiles;
 int i,size;
 int filePos;
 ifile=fopen(argv[1],"rb");
 if (!ifile)
    {printf("barf\n");
     exit(0);
    }
 fread(buff,1,12,ifile);
 fread(&nmFiles,4,1,ifile);
 printf("nmFiles=%d\n",nmFiles);
 filePos=12+4+16*nmFiles;
 for (i=0;i<nmFiles;i++)
    {fread(name,12,1,ifile);
     name[12]=0;
     printf("file=%s\n",name);     
     fread(&size,4,1,ifile);
     if (1)
	{FILE *f2,*fout;
	 f2=fopen(argv[1],"rb");
	 fseek(f2,filePos,0);
	 fread(buff,size,1,f2);
	 fout=fopen(name,"wb");
	 fwrite(buff,size,1,fout);
	 fclose(fout);
	 fclose(f2);	 
	}
     filePos+=size;	
    }
}
