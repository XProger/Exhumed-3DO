#ifndef __INCLUDEDpicseth
#define __INCLUDEDpicseth

unsigned char *loadPic(int fd,int *width,int *height);
int loadPicSet(int fd,unsigned short **palletes,
	       unsigned int **datas,int maxNmPics);
void skipPicSet(int fd);

#endif
