#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<conio.h>

#define N 4096

void main(void)
{int i;
 unsigned short j;
 FILE *ofile;
 ofile=fopen("..\\rndtab.h","wb");
 fprintf(ofile,"#define RANDTABLESIZE %d\n",N);
 fprintf(ofile,"static unsigned int randTable[RANDTABLESIZE]={\n");
 for (i=0;i<N;i++)
    {j=0;
     while (!kbhit()) j++;
     getch();
     printf("#%d - %d\n",i,j);
     fprintf(ofile,"%uU,",j);
     if (!(i & 0xf))
	fprintf(ofile,"\n");
    }
 fprintf(ofile,"};\n");
}
