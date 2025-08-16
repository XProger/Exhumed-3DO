#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

FILE *ofile,*ifile,*mapFile;

char map[256][64];
int mapSize;

char line[256];

int main(int argc,char **argv)
{int i,ret,state,number,lineNm;
 char *c,letter;
 if (argc!=4)
    {printf("Args! infile outfile mapfile\n");
     exit(-1);
    }
 ifile=fopen(argv[1],"r");
 mapFile=fopen(argv[3],"r");
 ofile=fopen(argv[2],"wb");
 if (!ifile || !mapFile || !ofile)
    {printf("Yak!\n");
     exit(-1);
    }
 /* read map file */
 mapSize=0;
 while (!feof(mapFile))
    {if (!fgets(map[mapSize++],64,mapFile))
	break;
     assert(mapSize<255);
    }
 for (i=0;i<mapSize;i++)
    {if (strchr(map[i],'.'))
	*strchr(map[i],'.')=0;
    }
 /* read input file */
 lineNm=0;
 while (!feof(ifile))
    {if (!fgets(line,256,ifile))
	break;
     printf("Processing line #%d...\n",lineNm);
     if (line[0]!='#')
	{fputs(line,ofile);
	 lineNm++;
	 continue;
	}
     state=0;
     for (c=line+1;*c;c++)
	{switch (state)
	    {case 0:
		if (*c>='A' && *c<='H')
		   {letter=*c;
		    number=0;
		    state=1;
		    break;
		   }
		if (*c=='\n')
		   break;
		fputc(*c,ofile);
		break;
	     case 1:
		if (*c>='0' && *c<='9')
		   {number*=10;
		    number+=*c-'0';
		    break;
		   }
		if (*c==',' || *c=='\n')
		   {char buff[64];
		    sprintf(buff,"%c-%d",letter,number);
		    for (i=0;i<mapSize;i++)
		       {if (!stricmp(map[i],buff))
			   break;
		       }
		    if (i>=mapSize)
		       {printf("couldn't find %c-%d\n",letter,number);
			exit(-1);
		       }
		    fputc(i,ofile);
		    state=0;
		    break;
		   }
		assert(0);
		break;
	       }
	 
	}
     fputc('\n',ofile);
     lineNm++;
    }
 fclose(ofile);
 fclose(ifile);
 fclose(mapFile); 
 return 0;
}
