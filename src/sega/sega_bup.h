#ifndef SEBA_BUP_H
#define SEGA_BUP_H

#ifdef TODO
#endif

typedef struct {
    sint32 TODO;
} BupConfig;

typedef struct {
    sint32 freeblock;
} BupStat;

typedef struct {
    sint32 year;
    sint32 month;
    sint32 day;
    sint32 time;
    sint32 min;
} BupDate;

typedef struct {
    char filename[128];
    char comment[128];
    sint32 datasize;
    sint32 blocksize;
    sint32 language;
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