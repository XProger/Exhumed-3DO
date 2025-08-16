#include <stdlib.h>
#include <stdio.h>

unsigned char picdata[512][512];
unsigned short picPal[256];
int picWidth,picHeight;

FILE *ofile;

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
 for (i=picHeight-1;i>=0;i--)
    {fread(picdata[i],(picWidth+3)&0xfffffffc,1,ifile);
    }
 fclose(ifile);
}

void writeInt(int i)
{fprintf(ofile,"%d,",i >> 24);
 fprintf(ofile,"%d,",(i >> 16)&0xff);
 fprintf(ofile,"%d,",(i >> 8)&0xff);
 fprintf(ofile,"%d,",i&0xff);
}

void writeShort(int i)
{fprintf(ofile,"%d,",(i >> 8)&0xff);
 fprintf(ofile,"%d,",i&0xff);
}

void writeChar(unsigned char i)
{fprintf(ofile,"%d,",i);
}

int main(int argc,char **argv)
{int x,y,count=0;
 readBmp(argv[1]);
 ofile=fopen(argv[2],"a");
 if (!ofile)
    {printf("Barf!\n");
     exit(-1);
    }
 fprintf(ofile,"unsigned char %s[] = {",argv[3]);
 for (x=0;x<256;x++)
    writeShort(picPal[x]);
 fprintf(ofile,"};\n");
 fclose(ofile);
 return 0;
}
