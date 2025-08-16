#include <libsn.h>
#include "file.h"

void fs_init(void)
{PCinit();
}

int fs_open(char *filename)
{return PCopen(filename,0,0);
}

int fs_read(int fd, char *buf, int n)
{int v;
 v=PCread(fd,buf,n);
 return v;
}

int fs_close(int fd)
{return PCclose(fd);
}

int fs_len(int fd)
{int size;
 size=PClseek(fd,0,2);
 PClseek(fd,0,0);
 return size;
}
