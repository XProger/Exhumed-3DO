#ifndef __INCLUDEDprofileh
#define __INCLUDEDprofileh

#ifndef NPROFILE

void initProfiler(void);
void popProfile(void);
void pushProfile(char *id);
void dumpProfileData(void);

#else

#define initProfiler()
#define popProfile()
#define pushProfile(x)
#define dumpProfileData()

#endif

#endif
