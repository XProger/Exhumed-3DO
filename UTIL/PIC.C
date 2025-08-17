#include <stdio.h>
#include <stdlib.h>

unsigned char data[200000];

int main(int argc,char **argv)
{FILE *ifile,*ofile;
 int width,height,nmpal,i;
 if (argc!=3)
    {printf("Barf!\n");
     return -1;
    }
 ifile=fopen(argv[1],"rb");
 ofile=fopen(argv[2],"ab");
 if (!ifile || !ofile)
    {printf("Barf!\n");
     return -1;
    }
 fread(data,14+4,1,ifile);
 fread(&width,4,1,ifile);
 fread(&height,4,1,ifile);
 fseek(ifile,14+40+1024,0);

 fread(data,200000,1,ifile);
 printf("width=%d height=%d\n",width,height);

 data[0]=width>>8;
 data[1]=width&0xff;
 data[2]=height>>8;
 data[3]=height&0xff;
 fwrite(data,4,1,ofile);

 for (i=0;i<height*width;i++)
    {if (data[i]==0)
	{data[i]=255;
	 continue;
	}
     if (data[i]==255)
	data[i]=0;
    }
 for (i=0;i<height;i++)
    fwrite(data+(height-i-1)*((width+3)&0xfffffffc),width,1,ofile);
 fclose(ofile);
 fclose(ifile);
 return 0;
}
