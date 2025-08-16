#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

#define MAXLABEL 256

int LabelCount;   

int LabelFirst[MAXLABEL];  

#define MAXBLOCK 256

int BlockSize[MAXBLOCK];

#define MAXBLOCKSIZE 16384

char BlockChunk[MAXBLOCK][MAXBLOCKSIZE];

int BlockCount;   

int main(int argc, char **argv)
{
#if 1
 char line[256];
 FILE *filein, *fileout;
 char *pos;
 long InWord, InBlock, i, j;
 unsigned offset, tmp;
    
 LabelCount = BlockCount = 0;    
 LabelFirst[0] = 0;
 InWord = InBlock = FALSE;

 if (argc!=3)
    {printf("Error!\n");
     exit(-1);
    }
 filein=fopen(argv[1], "rt");
 if (!filein)
    {printf("Error!\n");
     exit(-1);
    }
 printf("\n");
 while (fgets(line, 256, filein))
    {/* printf("Read line:'%s'\n",line); */
     switch (*line)
	{case ':':
	   line[strlen(line)-1] = '\0';
	   if (LabelCount > 0)
	      printf(" -> Contains %d blocks\n", 
		     BlockCount-LabelFirst[LabelCount-1]);
	   printf("Processing '%s' label...\n", line+1);    
	   LabelFirst[LabelCount++] = BlockCount;
	case '\n':
	case ';':
	   if (InBlock)
	      {BlockCount++;
	       InWord = InBlock = FALSE;
	      }
	   break;
	   default :
	      if (!InBlock)
		 {BlockSize[BlockCount] = 0;
		 }
	      else
		 {if (InWord)
		     {BlockChunk[BlockCount]
			 [BlockSize[BlockCount]++] = ' ';
		     }    
		 }
	   InBlock = TRUE;
	   InWord = FALSE;
	   for (pos=line; *pos; pos++)
	      {switch (*pos)
		  {case ('\n') :
		   case (' ') :
		      if (InWord)
			 {InWord = FALSE;
			  BlockChunk[BlockCount]
			     [BlockSize[BlockCount]++] = ' ';
			 }    
		      
		      break;
		      
		   case ('\\') :
		      
		      if (*(pos+1) == 'n')
			 {pos++;
			  
			  if (InWord)
			     {InWord = FALSE;
			     }    
			  BlockChunk[BlockCount]
			     [BlockSize[BlockCount]++] = '\n';
			 }
		      
		      break;
		      default :        
			 BlockChunk[BlockCount]
			    [BlockSize[BlockCount]++] = *pos;
		      InWord = TRUE;    
		      break;
		     }
	      }
	   break;
	  }
    }
 
 if (LabelCount > 0)
    printf(" -> Contains %d blocks\n", 
	   BlockCount-LabelFirst[LabelCount-1]);
 LabelFirst[LabelCount] = BlockCount;
 
 printf("\nLabels : %d BlockCount : %d\n", LabelCount, BlockCount);
 
fileout=fopen(argv[2], "wb");
 offset = 4 * LabelCount + 4 * BlockCount;
 
 for (i=0; i<BlockCount; i++)
    {offset += BlockSize[i];
    }
 
 tmp = ((offset & 0xff) << 24) | ((offset & 0xff00) << 8) |
    ((offset & 0xff0000) >> 8) | (offset >> 24);
 
 fwrite (&tmp, sizeof(tmp), 1, fileout);
 
 
 offset = 4 * LabelCount;
 
 for (i=0 ; i<LabelCount; i++)
    {tmp = ((offset & 0xff) << 24) | ((offset & 0xff00) << 8) |
	((offset & 0xff0000) >> 8) | (offset >> 24);
     fwrite (&tmp, sizeof(tmp), 1, fileout);
     offset += 4 * (LabelFirst[i+1] - LabelFirst[i]);
    }    
 for (i=0 ; i<BlockCount; i++)
    {tmp = ((offset & 0xff) << 24) | ((offset & 0xff00) << 8) |
	    ((offset & 0xff0000) >> 8) | (offset >> 24);
     
     fwrite (&tmp, sizeof(tmp), 1, fileout);
     
     offset += (BlockSize[i]);
    }    
 
 
 for (i=0 ; i<BlockCount; i++)
    {BlockChunk[i][BlockSize[i]-1] = '\0';
     
     for (j=0; j<BlockSize[i]; j++)
	{fwrite (&BlockChunk[i][j], sizeof(char), 1, fileout);
	}
    }    
 
 fclose (fileout);
 fclose (filein);
#endif
 return 0;
}


