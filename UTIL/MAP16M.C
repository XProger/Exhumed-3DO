#include <stdio.h>
#include <stdlib.h>

int picWidth,picHeight;
unsigned char picdata[1024][1024][3];

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
 printf("Pic is %d x %d\n",picWidth,picHeight);

 for (i=picHeight-1;i>=0;i--)
    {fread(picdata[i],(picWidth*3+3)&0xfffffffc,1,ifile);
    }
 fclose(ifile);
}

FILE *ofile;

void writeInt(int i)
{fputc(i >> 24,ofile);
 fputc((i >> 16)&0xff,ofile);
 fputc((i >> 8)&0xff,ofile);
 fputc(i & 0xff,ofile);
}

void writeShort(short i)
{fputc(i >> 8,ofile);
 fputc(i & 0xff,ofile);
}

void writeChar(char i)
{fputc(i,ofile);
}

void main(void)
{int x,y,x1,y1,c,count;
 readBmp("c:\\sr3\\util\\ow14.bmp");
 ofile=fopen("c:\\sr3\\data\\map.dat","ab");
 count=0;
 for (y=0;y<512;y+=64)
    for (x=0;x<640;x+=64)
       count++;

 writeInt(count);

 for (y=0;y<512;y+=64)
    for (x=0;x<640;x+=64)
       {writeShort(0);
	for (y1=0;y1<64;y1++)
	   for (x1=0;x1<64;x1++)
	      {c=0x8000|
		  ((picdata[y+y1][x+x1][0]>>3)<<10)|
		     ((picdata[y+y1][x+x1][1]>>3)<<5)|
			((picdata[y+y1][x+x1][2]>>3)<<0);
	       writeShort(c);
	      }
       }
}
