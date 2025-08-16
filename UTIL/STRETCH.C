#include <stdio.h>
#include <stdlib.h>

unsigned char picdata[512][512],outdata[512][512];
unsigned short picPal[256];
unsigned short lastPallete[256];
int picWidth,picHeight;

FILE *ofile;

unsigned char head1[80],head2[80],pall[256*4];

void readBmp(char *filename)
{FILE *ifile;
 int width,height,i,j;
 ifile=fopen(filename,"rb");
 if (!ifile)
    {printf("Error: can't read file %s.\n",filename);
     exit(-1);
    }
 fread(head1,14+4,1,ifile);
 fread(&width,4,1,ifile);
 fread(&height,4,1,ifile);
 picWidth=width;
 picHeight=height;
 fread(head2,28,1,ifile);
 if (picWidth>512 || picHeight>512)
    {printf("Pic toooo big\n");
     exit(-1);
    }
 /* read pallete */
 fread(pall,256*4,1,ifile);

 for (i=picHeight-1;i>=0;i--)
    {fread(picdata[i],(picWidth+3)&0xfffffffc,1,ifile);
     for (j=picWidth;j<=((picWidth+7)&(~7));j++)
	picdata[i][j]=255;
    }
 fclose(ifile);
}


void writeBmp(char *filename)
{FILE *ofile;
 int i;
 ofile=fopen(filename,"wb");
 if (!ofile)
    {printf("Can't open %s for writing\n",filename);
     exit(-1);
    }
 *(int *)(head1+2)=
    ((((picWidth+3)&(~3))*picHeight+(14+4+4+4+28+256*4)));
 fwrite(head1,14+4,1,ofile);
 fwrite(&picWidth,4,1,ofile);
 fwrite(&picHeight,4,1,ofile);
 *(int *)(head2+8)=
    ((picWidth+3)&(~3))*picHeight;
 fwrite(head2,28,1,ofile);
 fwrite(pall,256*4,1,ofile);
 for (i=picHeight-1;i>=0;i--)
    fwrite(outdata[i],(picWidth+3)&(~3),1,ofile);
 fclose(ofile);
}

void main(int argc,char **argv)
{int horzStretch=0;
 int vertStretch=0;
 int i,vscale,hscale;
 char *infile;
 infile=NULL;
 for (i=1;i<argc;i++)
    {if (!stricmp(argv[i],"h"))
	{horzStretch=1;
	 continue;
	}
     if (!stricmp(argv[i],"v"))
	{vertStretch=1;
	 continue;
	}
     infile=argv[i];
    }
 if (!infile)
    {printf("Puke!\n");
     exit(-1);
    }
 readBmp(infile);
 vscale=64/picHeight;
 if (vscale*picHeight!=64)
    {printf("Pic height not good.\n");
     exit(-1);
    }
 hscale=64/picWidth;
 if (hscale*picWidth!=64)
    {printf("Pic width not good.\n");
     exit(-1); 
    }
 {int x,y;
  int xt,yt;
  for (y=0;y<64;y++)
     {if (vertStretch)
	 yt=y/vscale;
      else
	 yt=y % picHeight;
      for (x=0;x<64;x++)
	 {if (horzStretch)
	     xt=x/hscale;
	  else
	     xt=x % picWidth;
	  outdata[y][x]=picdata[yt][xt];
	 }
     }
  picWidth=64;
  picHeight=64;
  writeBmp(infile);
 }
}
