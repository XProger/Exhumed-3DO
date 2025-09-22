#ifndef SEBA_BUP_H
#define SEGA_BUP_H

#ifdef TODO
#endif

typedef struct {
    int TODO;
} BupConfig;

typedef struct {
    int freeblock;
} BupStat;

typedef struct {
    int year;
    int month;
    int day;
    int time;
    int min;
} BupDate;

typedef struct {
    char filename[128];
    char comment[128];
    int datasize;
    int blocksize;
    int language;
    BupDate *date;
} BupDir;

#define BUP_SetDate(date_ptr)   (date_ptr)
#define BUP_ENGLISH 0
#define BUP_BROKEN 0
#define BUP_UNFORMAT 0

#define OFF 0

#define BUP_Init(bupSpace,bupWork,config) (0)
#define BUP_Delete(id,FILENAME) (0)
#define BUP_Read(id,key,saveGames) (0)
#define BUP_Write(device,data1,data2,flag) (0)
#define BUP_Stat(id,size,sttb) ((sttb)->freeblock = size)
#define BUP_Format(id) (0)

#endif