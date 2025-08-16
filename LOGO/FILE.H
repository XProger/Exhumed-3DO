#ifndef __INCLUDEDfileh
#define __INCLUDEDfileh

void fs_init(void);
int fs_open(char *filename);
int fs_read(int fd, char *buf,int n);
int fs_close(int fd);
int fs_len(int fd);

#endif
