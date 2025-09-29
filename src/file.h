#ifndef __INCLUDEDfileh
#define __INCLUDEDfileh

void fs_startProgress(sint32 swirly);
void fs_addToProgress(char* filename);
void fs_closeProgress(void);

void fs_init(void);
sint32 fs_open(char* filename);
void fs_read(sint32 fd, void* buf, sint32 n);
void fs_close(sint32 fd);
void fs_execOne(void);

void playWholeCD(void);
void playCDTrack(sint32 track, sint32 repeat);
void stopCD(void);

sint32 getTrackStartFAD(sint32 track);
sint32 getCurrentFAD(void);
sint32 getCurrentStatus(void);

void link(char* filename);

#endif
