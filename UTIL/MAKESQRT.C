#include<stdlib.h>
#include<math.h>
#include<stdio.h>

#define SQRTTABLESIZE 1024
/* this must be even */
#define SQRTTABLEBITS 10
#define SQRTTABLEMASK 0x3ff

void main(void)
{int i;
 unsigned j;
 FILE *ofile;
 ofile=fopen("..\\sqrttab.h","wb");
 fprintf(ofile,"static unsigned int sqrtTable[SQRTTABLESIZE]={\n");
 for (i=0;i<SQRTTABLESIZE;i++)
    {j=(65536.0*65536.0)*sqrt(((double)i)/((double)SQRTTABLESIZE));
     fprintf(ofile,"%uU,",j);
     if (!(i & 0xf))
	fprintf(ofile,"\n");
    }
 fprintf(ofile,"};\n");
}
