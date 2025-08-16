#include <machine.h>

#include <sega_spr.h>
#include <sega_scl.h>
#include <sega_int.h>
#include <sega_mth.h>
#include <sega_sys.h>
#include <sega_dbg.h>
#include <sega_per.h>
#include <sega_dma.h>
#include <string.h>

#include "util.h"
#include "file.h"
#include "picset.h"


unsigned char *loadPic(int fd,int *width,int *height)
{short w,h;
 short flags;
 unsigned char *data;
 fs_read(fd,(char *)&w,2);
 fs_read(fd,(char *)&h,2);
 fs_read(fd,(char *)&flags,2);
 *width=w;
 *height=h;
 data=mem_malloc(0,*width**height);
 assert(data);
 fs_read(fd,data,*width**height);
 return data;
}

void skipPicSet(int fd)
{int size;
 char *data;
 fs_read(fd,(char *)&size,4);
 data=mem_malloc(0,size);
 fs_read(fd,data,size);
 mem_free(data);
}

int loadPicSet(int fd,unsigned short **palletes,
	       unsigned int **datas,int maxNmPics)
{char *data;
 char *d;
 char *lastPallete;
 int chunkSize;
 int size;
 int pic,w,h;
 fs_read(fd,(char *)&size,4);
 data=mem_malloc(0,size);
 fs_read(fd,data,size);
 pic=0;
 d=data;
 lastPallete=NULL;
 while (d<data+size)
    {assert(!(((int)d) & 3));
     datas[pic]=((unsigned int *)d)+1;
     chunkSize=*(int *)d;
     w=*(((int *)d)+1);
     h=*(((int *)d)+2);
     d+=chunkSize+12;
     if ((*(int *)d) & 1)
	{d+=4;
	 palletes[pic]=(unsigned short *)d;
	 lastPallete=d;
	 d+=256*2;
	}
     else
	{d+=4;
	 assert(lastPallete);
	 palletes[pic]=(unsigned short *)lastPallete;
	}
     pic++;
     assert(pic<maxNmPics);
    }
 return pic;
}

