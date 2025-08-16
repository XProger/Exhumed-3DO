#include "util.h"
#include <sega_per.h>
#include "file.h"


static int *textData;

int getLanguageNumber(void)
{int skip;
 skip=0;
 switch (systemMemory & PER_MSK_LANGU)
    {case PER_ENGLISH: skip=0; break;
     case PER_ESPNOL: skip=1; break;
     case PER_FRANCAIS: skip=2; break;
     case PER_DEUTSCH: skip=3; break;
    }
 return skip;
}

void loadLocalText(int fd)
{int size,skip;
 textData=mem_malloc(0,5*1024);
 mem_lock();
 skip=getLanguageNumber();
 for (;skip>=0;skip--)
    {fs_read(fd,(char *)&size,4);
     assert(size<5*1024);       
     fs_read(fd,(char *)textData,size);
    }
}

char *getText(int block,int string)
{int addr;
 addr=((int)textData)+textData[(textData[block]>>2)+string];
 return (char *)addr;
}

