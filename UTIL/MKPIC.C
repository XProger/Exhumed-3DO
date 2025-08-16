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

int main(int argc,char **argv)
{int i,x,y;
 if (argc!=3)
    {printf("Arg!\n");
     exit(-1);
    }
 ofile=fopen(argv[2],"wb");
 readBmp(argv[1]);
 for (i=0;i<256;i++)
    writeShort(picPal[i]);
 writeInt(picWidth);
 writeInt(picHeight);
 for (y=0;y<picHeight;y++)
    for (x=0;x<picWidth;x++)
       writeChar(picdata[y][x]);
 fclose(ofile);
 return 0;
}
