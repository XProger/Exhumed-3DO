#include <stdio.h>
#include <stdlib.h>

#include "fnt_dat1.c"

int main(int argc,char **argv)
{int x,y,ch,cx,cy;
 FILE *ofile;
 ofile=fopen("kana.pnm","wb");
 fprintf(ofile,"P6\n320 200\n255\n");
 for (y=0;y<200;y++)
    for (x=0;x<320;x++)
       {ch=(y>>5)*20+(x>>4);
	cx=x&0xf;
	cy=y&0x1f;
	if (cx<8 && cy<16 &&
	    ((FntKanaFontData[ch*16+cy]>>cx)&1))
	   fprintf(ofile,"111");
	else
	   fprintf(ofile,"zzz");
       }
 return 0;
}
