#ifndef __INCLUDEDfileh
#define __INCLUDEDfileh

void fs_startProgress(int swirly);
void fs_addToProgress(char* filename);
void fs_closeProgress(void);

void fs_init(void);
int fs_open(char* filename);
void fs_read(int fd, char* buf, int n);
void fs_close(int fd);
void fs_execOne(void);

void playWholeCD(void);
void playCDTrack(int track, int repeat);
void stopCD(void);

int getTrackStartFAD(int track);
int getCurrentFAD(void);
int getCurrentStatus(void);

void link(char* filename);

#define FS_INT(p)   (Sint32)( ((Uint8*)(p))[3] | (((Uint8*)(p))[2] << 8) | (((Uint8*)(p))[1] << 16) | (((Uint8*)(p))[0] << 24) )
#define FS_SHORT(p) (Sint32)( ((Uint8*)(p))[1] | (((Uint8*)(p))[0] << 8) )

#endif
